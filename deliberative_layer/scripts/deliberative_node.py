#!/usr/bin/env python3
"""
Deliberative layer ROS2 node.
Uses FF to plan from PDDL domain/problem, sends plan to skill_execution_manager.
On failure: reads current resource configuration from response, updates problem init,
re-plans with FF. Does not alter the domain. Stops after more than 3 re-planning cycles.
"""

import json
import shutil
import tempfile
from pathlib import Path

import rclpy
from rclpy.node import Node

from skill_execution_manager.srv import ExecutePlan
from test_campaign_msgs.msg import ExecutionTrace

from deliberative_layer.problem_updater import update_problem_init_with_config
from deliberative_layer.ff_planner import run_ff


class DeliberativeNode(Node):
    def __init__(self):
        super().__init__("deliberative_node")
        self.declare_parameter("domain_path", "")
        self.declare_parameter("problem_path", "")
        self.declare_parameter("pddl_model_folder", 0)
        self.declare_parameter("ff_binary", "ff")
        self.declare_parameter("execution_manager_service", "/execute_plan")
        self.declare_parameter("max_retries", 3)
        self.declare_parameter("execution_trace_topic", "")

        trace_topic = self.get_parameter("execution_trace_topic").value
        self._trace_pub = None
        if trace_topic and str(trace_topic).strip():
            self._trace_pub = self.create_publisher(ExecutionTrace, str(trace_topic).strip(), 10)
            self.get_logger().info(f"Execution trace publishing to {trace_topic}")

        self._client = self.create_client(
            ExecutePlan,
            self.get_parameter("execution_manager_service").value,
        )
        self.get_logger().info("Waiting for skill_execution_manager ExecutePlan service...")
        if not self._client.wait_for_service(timeout_sec=60.0):
            self.get_logger().error("ExecutePlan service not available. Is execution_manager_node running?")
            return

        self._run_plan_loop()

    def _run_plan_loop(self) -> None:
        domain_val = self.get_parameter("domain_path").value or ""
        problem_val = self.get_parameter("problem_path").value or ""
        pddl_folder = int(self.get_parameter("pddl_model_folder").value)

        if not str(domain_val).strip():
            try:
                from ament_index_python.packages import get_package_share_directory
                share = Path(get_package_share_directory("deliberative_layer"))
                domain_path = share / "PDDL_models" / str(pddl_folder) / "quadruped_example_domain.pddl"
            except Exception:
                domain_path = Path(domain_val) if domain_val else None
        else:
            domain_path = Path(domain_val)

        if not str(problem_val).strip():
            try:
                from ament_index_python.packages import get_package_share_directory
                share = Path(get_package_share_directory("deliberative_layer"))
                problem_path = share / "PDDL_models" / str(pddl_folder) / "quadruped_example_problem.pddl"
            except Exception:
                problem_path = Path(problem_val) if problem_val else None
        else:
            problem_path = Path(problem_val)
        ff_binary = self.get_parameter("ff_binary").value
        max_retries = int(self.get_parameter("max_retries").value)

        if domain_path is None or not domain_path.exists():
            self.get_logger().error(f"Domain not found: {domain_path}")
            return
        if problem_path is None or not problem_path.exists():
            self.get_logger().error(f"Problem not found: {problem_path}")
            return

        work_dir = Path(tempfile.mkdtemp())
        domain_path = Path(domain_path)
        current_problem = work_dir / "problem.pddl"
        shutil.copy(problem_path, current_problem)

        def _trace(event: str, data: dict) -> None:
            if self._trace_pub is not None:
                msg = ExecutionTrace()
                msg.event = event
                msg.data = json.dumps(data)
                self._trace_pub.publish(msg)

        replan_count = 0
        while replan_count <= max_retries:
            self.get_logger().info(
                f"Planning (replan_count={replan_count}, max_retries={max_retries})"
            )

            plan_path = run_ff(domain_path, current_problem, ff_binary, work_dir)
            if plan_path is None:
                self.get_logger().error("FF could not find a plan.")
                _trace("plan_complete", {"success": False, "reason": "no_plan", "replan_count": replan_count})
                break

            self.get_logger().info(f"Plan found: {plan_path}")

            plan_actions = []
            try:
                from skill_execution_manager_core.plan import Plan
                plan = Plan.from_ff_file(str(plan_path))
                plan_actions = [s.action for s in plan.steps()]
            except Exception:
                pass
            _trace("plan_start", {"plan": plan_actions, "replan_count": replan_count})

            req = ExecutePlan.Request()
            req.plan_ff_path = str(plan_path)
            req.robust_model_path = ""

            future = self._client.call_async(req)
            rclpy.spin_until_future_complete(self, future, timeout_sec=300.0)

            if not future.done():
                self.get_logger().error("ExecutePlan call timed out.")
                _trace("plan_complete", {"success": False, "reason": "timeout", "replan_count": replan_count})
                break

            response = future.result()
            if response.success:
                self.get_logger().info("Plan completed successfully.")
                _trace("plan_complete", {"success": True, "replan_count": replan_count})
                break

            self.get_logger().warn(
                f"Plan failed: {response.failure_reason} on action {response.failed_action}"
            )
            if replan_count >= max_retries:
                self.get_logger().error(
                    "Re-planning occurred more than 3 times. Dropping plan and stopping."
                )
                _trace("plan_complete", {
                    "success": False,
                    "reason": "max_replans",
                    "replan_count": replan_count,
                    "failed_action": response.failed_action,
                    "failure_reason": response.failure_reason,
                })
                break

            try:
                config = json.loads(response.config) if response.config else {}
            except json.JSONDecodeError:
                config = {}
            if not config:
                self.get_logger().error("No configuration in failure response. Cannot re-plan.")
                _trace("plan_complete", {
                    "success": False,
                    "reason": "no_config",
                    "replan_count": replan_count,
                    "failed_action": response.failed_action,
                })
                break

            next_problem = work_dir / f"problem_retry_{replan_count + 1}.pddl"
            update_problem_init_with_config(current_problem, config, next_problem)
            _trace("replan", {
                "trigger_action": response.failed_action,
                "trigger_reason": response.failure_reason,
                "config_snapshot": config,
                "replan_count": replan_count + 1,
            })
            current_problem = next_problem
            replan_count += 1
            self.get_logger().info(
                f"Updated problem with config {config}, re-planning (attempt {replan_count + 1})..."
            )

        try:
            shutil.rmtree(work_dir, ignore_errors=True)
        except Exception:
            pass


def main(args=None):
    rclpy.init(args=args)
    node = DeliberativeNode()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == "__main__":
    main()
