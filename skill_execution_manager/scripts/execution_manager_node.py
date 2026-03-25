#!/usr/bin/env python3
"""
ROS2 node for the skill execution manager.
Connects to quadruped_impl_node (skillset_manager="quadruped_example") to execute skills.
Exposes ExecutePlan service: given a path to a .ff plan, runs the plan and returns success/failure.
"""

import json
import threading
import time
from pathlib import Path

import rclpy
from rclpy.node import Node
from rclpy.executors import MultiThreadedExecutor

from skill_execution_manager.srv import ExecutePlan
from test_campaign_msgs.msg import ExecutionTrace
from quadruped_example_skillset_client import QuadrupedExampleSkillsetClient
from skill_execution_manager_core import (
    SkillExecutionManager,
    RobustModel,
    Plan,
    SkillExecutor,
)
from skill_execution_manager_core.state_checker import state_from_resources


def _make_observer(client):
    """Build observer that requests fresh status and includes location for escalation config."""

    def observe():
        # Double refresh to avoid stale SkillsetStatus from other subscribers (e.g. GUI)
        client.refresh_status()
        time.sleep(0.05)
        client.refresh_status()
        state = dict(state_from_resources(client.resources))
        try:
            loc_data = client.data.location.get()
            if loc_data and loc_data.value:
                loc_str = getattr(loc_data.value, "data", None) or str(loc_data.value)
                if loc_str and loc_str.strip():
                    state["at_location"] = loc_str.strip()
        except Exception:
            pass
        return state

    return observe


def get_package_share_path(package: str) -> Path:
    """Return share directory of a package."""
    from ament_index_python.packages import get_package_share_directory
    return Path(get_package_share_directory(package))


class ExecutionManagerNode(Node):
    def __init__(self):
        super().__init__("execution_manager_node")
        self._ros_executor = MultiThreadedExecutor()
        self._ros_executor.add_node(self)
        self.declare_parameter("skillset_manager", "quadruped_example")
        self.declare_parameter("robust_model_path", "")
        self.declare_parameter("default_plan_dir", "")
        self.declare_parameter("execution_trace_topic", "")

        trace_topic = self.get_parameter("execution_trace_topic").value
        self._trace_pub = None
        if trace_topic and str(trace_topic).strip():
            self._trace_pub = self.create_publisher(ExecutionTrace, str(trace_topic).strip(), 10)
            self.get_logger().info(f"Execution trace publishing to {trace_topic}")

        self._client = None
        self._executor = None
        self._observer = None
        self._client_error = None
        self._client_ready = threading.Event()
        self._client_lock = threading.Lock()

        self._srv = self.create_service(
            ExecutePlan,
            "/execute_plan",
            self._handle_execute_plan,
        )
        self.get_logger().info("ExecutePlan service ready on /execute_plan")
        # Start connecting to skillset in background (blocks on quadruped_impl handshake).
        # Delay lets quadruped_impl start its subscribers before we publish status_request.
        # 5s ensures skillset is fully ready before deliberative (started at 15s) calls ExecutePlan.
        def connect_after_delay():
            time.sleep(5.0)
            self._connect_client()

        self._connect_thread = threading.Thread(target=connect_after_delay, daemon=True)
        self._connect_thread.start()

    def _connect_client(self):
        """Background thread: create skillset client (may block until quadruped_impl responds)."""
        try:
            skillset_manager = self.get_parameter("skillset_manager").value
            self.get_logger().info(f"Connecting to skillset_manager={skillset_manager}...")
            client = QuadrupedExampleSkillsetClient(
                node_name="skill_execution_manager_client",
                skillset_manager=skillset_manager,
                data_subscription=False,
                spin=False,  # We add client node to our executor instead
            )
            with self._client_lock:
                self._client = client
                self._executor = SkillExecutor(client)
                self._observer = _make_observer(client)
                self._ros_executor.add_node(client.node)
            self.get_logger().info("Connected to skillset_manager")
        except Exception as e:
            self._client_error = e
            self.get_logger().error(f"Failed to connect to skillset: {e}")
        finally:
            self._client_ready.set()

    def _ensure_client(self):
        """Wait for skillset client to be ready (with timeout)."""
        if self._client is not None:
            return
        if not self._client_ready.wait(timeout=30.0):
            raise TimeoutError(
                "Skillset client did not connect within 30s. "
                "Is quadruped_impl_node running and responding to status requests?"
            )
        if self._client_error is not None:
            raise self._client_error
        if self._client is None:
            raise RuntimeError("Skillset client connection failed.")

    def _handle_execute_plan(self, request, response):
        try:
            self._ensure_client()
        except Exception as e:
            response.success = False
            response.failure_reason = "SkillsetConnectionError"
            response.failed_action = ""
            response.config = json.dumps({"error": str(e)})
            self.get_logger().error(str(e))
            return response

        plan_path = Path(request.plan_ff_path)
        robust_path_str = request.robust_model_path.strip()
        if robust_path_str:
            robust_path = Path(robust_path_str)
        else:
            try:
                share = get_package_share_path("skill_execution_manager")
                robust_path = share / "robust_model" / "quadruped_example.json"
            except Exception:
                share = Path(__file__).resolve().parent.parent.parent
                robust_path = share / "robust_model" / "quadruped_example.json"

        if not plan_path.is_absolute():
            try:
                share = get_package_share_path("skill_execution_manager")
                plan_path = share / "PDDL_plan" / (plan_path.name or "plan.ff")
            except Exception:
                base = Path(__file__).resolve().parent.parent.parent
                plan_path = base / "PDDL_plan" / (plan_path.name or "plan.ff")

        if not plan_path.exists():
            response.success = False
            response.failure_reason = "PlanFileNotFound"
            response.failed_action = ""
            response.config = json.dumps({"path": str(plan_path)})
            self.get_logger().error(f"Plan not found: {plan_path}")
            return response

        if not robust_path.exists():
            response.success = False
            response.failure_reason = "RobustModelNotFound"
            response.failed_action = ""
            response.config = json.dumps({"path": str(robust_path)})
            self.get_logger().error(f"Robust model not found: {robust_path}")
            return response

        robust_model = RobustModel.from_file(str(robust_path))
        plan = Plan.from_ff_file(str(plan_path))

        failure_info = {"action": "", "config": {}, "reason": ""}

        def on_escalation(action, config, reason):
            failure_info["action"] = action
            failure_info["config"] = config
            failure_info["reason"] = reason.value if hasattr(reason, "value") else str(reason)
            self.get_logger().warn(f"Escalation: {action} {reason} {config}")

        def on_trace(event, data):
            if self._trace_pub is not None:
                msg = ExecutionTrace()
                msg.event = event
                msg.data = json.dumps(data)
                self._trace_pub.publish(msg)

        manager = SkillExecutionManager(
            plan,
            robust_model,
            self._executor,
            self._observer,
            on_progress=lambda p: self.get_logger().debug(f"Progress: {p}"),
            on_escalation=on_escalation,
            on_feedback=lambda msg: self.get_logger().info(msg),
            on_trace=on_trace,
        )
        completed = manager.run()

        response.success = completed
        response.failure_reason = failure_info["reason"] if not completed else ""
        response.failed_action = failure_info["action"] if not completed else ""
        response.config = json.dumps(failure_info["config"]) if not completed else "{}"
        return response


def main(args=None):
    rclpy.init(args=args)
    node = ExecutionManagerNode()
    try:
        node._ros_executor.spin()
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == "__main__":
    main()
