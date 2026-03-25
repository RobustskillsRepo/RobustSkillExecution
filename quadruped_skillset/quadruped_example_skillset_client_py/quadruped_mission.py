#!/usr/bin/env python3
"""
Mission script WITHOUT skill_execution_manager (naive approach).
Runs the same test campaign as the robust execution path, but with simple replanning on failure:
- Execute plan step by step via skillset client
- On skill failure: observe current config, update PDDL problem, replan with FF, retry with new plan
- Stops after max_replans (default 3) or on success

Publishes trace events (plan_start, skill_start, skill_end, replan, plan_complete) for test_campaign.
Use for comparison with the skill_execution_manager approach.
"""

import json
import signal
import shutil
import sys
import tempfile
import time
from pathlib import Path

import rclpy
from rclpy.executors import MultiThreadedExecutor
from rclpy.node import Node
from rclpy.signals import SignalHandlerOptions
from threading import Thread

from quadruped_example_skillset_client import QuadrupedExampleSkillsetClient
from skill_execution_manager_core import Plan, SkillExecutor
from skill_execution_manager_core.state_checker import state_from_resources
from deliberative_layer.problem_updater import update_problem_init_with_config
from deliberative_layer.ff_planner import run_ff

from test_campaign_msgs.msg import ExecutionTrace


def get_pddl_paths(pddl_folder: int = 0):
    """Return (domain_path, problem_path) from deliberative_layer share."""
    try:
        from ament_index_python.packages import get_package_share_directory
        share = Path(get_package_share_directory("deliberative_layer"))
        folder = str(pddl_folder)
        domain = share / "PDDL_models" / folder / "quadruped_example_domain.pddl"
        problem = share / "PDDL_models" / folder / "quadruped_example_problem.pddl"
        return domain, problem
    except Exception:
        return None, None


def observe_config(client):
    """Get current resource config (motor_state, spot_state, at_location) for replanning."""
    client.refresh_status()
    time.sleep(0.05)
    client.refresh_status()
    config = dict(state_from_resources(client.resources))
    try:
        loc_data = client.data.location.get()
        if loc_data and loc_data.value:
            loc_str = getattr(loc_data.value, "data", None) or str(loc_data.value)
            if loc_str and loc_str.strip():
                config["at_location"] = loc_str.strip()
    except Exception:
        pass
    return config


def achieved_predicates_from_step(action, params):
    """
    Infer achieved predicates from a single successful step.
    Returns dict to merge into accumulated achieved state.
    """
    achieved = {}
    action_lower = (action or "").lower()
    params = params or []
    if action_lower == "take_picture" and params:
        loc = str(params[0]).strip().upper()
        achieved["picture_taken_at"] = [loc]
    elif action_lower == "recharge":
        achieved["charged"] = True
    return achieved


def achieved_predicates_from_plan(plan, failed_at_index):
    """
    Infer achieved predicates (picture_taken_at, charged) from plan steps 0..failed_at_index-1.
    Returns dict to merge into config for update_problem_init_with_config.
    """
    achieved = {"picture_taken_at": [], "charged": False}
    for i in range(min(failed_at_index, len(plan.steps()))):
        step = plan.steps()[i]
        step_achieved = achieved_predicates_from_step(step.action, step.parameters)
        if "picture_taken_at" in step_achieved:
            for loc in step_achieved["picture_taken_at"]:
                if loc not in achieved["picture_taken_at"]:
                    achieved["picture_taken_at"].append(loc)
        if step_achieved.get("charged"):
            achieved["charged"] = True
    return achieved


def execute_plan(client, plan, pddl_folder, max_replans, trace_pub, post_skill_delay_sec=0.0):
    """
    Execute plan step by step. On failure: replan and retry.
    Returns (success: bool, replan_count: int).
    """
    def _trace(event: str, data: dict):
        if trace_pub is not None:
            msg = ExecutionTrace()
            msg.event = event
            msg.data = json.dumps(data)
            trace_pub.publish(msg)

    executor = SkillExecutor(client)
    domain_path, problem_path = get_pddl_paths(pddl_folder)
    if not domain_path or not domain_path.exists() or not problem_path or not problem_path.exists():
        print("ERROR: PDDL domain/problem not found. Cannot replan.")
        _trace("plan_complete", {"success": False, "reason": "no_pddl", "replan_count": 0})
        return False, 0

    work_dir = Path(tempfile.mkdtemp())
    current_problem = work_dir / "problem.pddl"
    shutil.copy(problem_path, current_problem)
    replan_count = 0
    # Accumulate achieved predicates across all plans (persists across replans)
    achieved_state = {"picture_taken_at": [], "charged": False}

    while replan_count <= max_replans:
        steps = plan.steps()
        _trace("plan_start", {"plan": [s.action for s in steps], "replan_count": replan_count})
        print(f"Executing plan ({len(steps)} steps, replan_count={replan_count})")

        step_index = 0
        for i, step in enumerate(steps):
            action = step.action
            params = step.parameters
            step_label = f"Step {i + 1}/{len(steps)}: {action}"
            print(f"  {step_label}...")

            _trace("skill_start", {"skill": action, "step_index": step_index, "params": params})
            ok = executor.execute_and_wait(action, params)
            if post_skill_delay_sec > 0:
                time.sleep(post_skill_delay_sec)  # Let fault injector publish and quadruped process
            _trace("skill_end", {"skill": action, "step_index": step_index, "success": ok})

            if ok:
                # Update accumulated achieved state on success
                step_achieved = achieved_predicates_from_step(action, params)
                if "picture_taken_at" in step_achieved:
                    for loc in step_achieved["picture_taken_at"]:
                        if loc not in achieved_state["picture_taken_at"]:
                            achieved_state["picture_taken_at"].append(loc)
                if step_achieved.get("charged"):
                    achieved_state["charged"] = True

            if not ok:
                print(f"  FAILED: {action}")
                if replan_count >= max_replans:
                    print(f"Max replans ({max_replans}) reached. Mission aborted.")
                    try:
                        shutil.rmtree(work_dir, ignore_errors=True)
                    except Exception:
                        pass
                    _trace("plan_complete", {
                        "success": False,
                        "reason": "max_replans",
                        "replan_count": replan_count,
                        "failed_action": action,
                    })
                    return False, replan_count

                config = observe_config(client)
                # Use accumulated achieved state (from all prior successful steps), not just current plan
                config.update(dict(achieved_state))
                print(f"  Config: {config}")
                if not config.get("motor_state") and not config.get("spot_state"):
                    print("  No config from resources. Cannot replan.")
                    try:
                        shutil.rmtree(work_dir, ignore_errors=True)
                    except Exception:
                        pass
                    _trace("plan_complete", {
                        "success": False,
                        "reason": "no_config",
                        "replan_count": replan_count,
                        "failed_action": action,
                    })
                    return False, replan_count

                next_problem = work_dir / f"problem_retry_{replan_count + 1}.pddl"
                update_problem_init_with_config(current_problem, config, next_problem)
                _trace("replan", {
                    "trigger_action": action,
                    "trigger_reason": "skill_failed",
                    "config_snapshot": config,
                    "replan_count": replan_count + 1,
                })

                plan_path = run_ff(domain_path, next_problem, "ff", work_dir)
                current_problem = next_problem
                replan_count += 1

                if plan_path is None:
                    print("  FF could not find a plan. Mission aborted.")
                    try:
                        shutil.rmtree(work_dir, ignore_errors=True)
                    except Exception:
                        pass
                    _trace("plan_complete", {
                        "success": False,
                        "reason": "no_plan",
                        "replan_count": replan_count,
                    })
                    return False, replan_count

                plan = Plan.from_ff_file(str(plan_path))
                print(f"  Replanned. New plan has {len(plan)} steps.")
                break  # Restart from step 0 with new plan
            step_index += 1
        else:
            # All steps succeeded
            try:
                shutil.rmtree(work_dir, ignore_errors=True)
            except Exception:
                pass
            _trace("plan_complete", {"success": True, "replan_count": replan_count})
            return True, replan_count

    try:
        shutil.rmtree(work_dir, ignore_errors=True)
    except Exception:
        pass
    _trace("plan_complete", {"success": False, "reason": "max_replans", "replan_count": replan_count})
    return False, replan_count


def main():
    # Disable rclpy's SIGTERM handler so we can exit(0) when test_campaign kills us
    rclpy.init(signal_handler_options=SignalHandlerOptions.NO)
    signal.signal(signal.SIGTERM, lambda s, f: sys.exit(0))

    # Create node for parameters and trace publisher
    node = Node("quadruped_mission")
    node.declare_parameter("execution_trace_topic", "execution_trace")
    node.declare_parameter("pddl_model_folder", 0)
    node.declare_parameter("post_skill_delay_sec", 0.0)
    trace_topic = node.get_parameter("execution_trace_topic").value
    pddl_folder = int(node.get_parameter("pddl_model_folder").value)
    post_skill_delay = float(node.get_parameter("post_skill_delay_sec").value)

    trace_pub = node.create_publisher(ExecutionTrace, trace_topic, 10)

    print("ROS initialized")
    time.sleep(3.0)

    client = QuadrupedExampleSkillsetClient(
        node_name="quadruped_robot_mission",
        skillset_manager="quadruped_example",
        data_subscription=False,
        spin=False,
    )
    print("Skill client initialized")
    time.sleep(3.0)

    executor = MultiThreadedExecutor()
    executor.add_node(client.node)
    executor.add_node(node)
    executor_thread = Thread(target=executor.spin)
    executor_thread.start()

    time.sleep(2.0)  # Let executor process and connect to skillset
    client.refresh_status()
    print("Mission started (naive: replan on failure, no skill_execution_manager)")

    domain_path, problem_path = get_pddl_paths(pddl_folder)
    if not domain_path or not problem_path or not domain_path.exists() or not problem_path.exists():
        print("ERROR: PDDL files not found. Install deliberative_layer and ensure PDDL_models are installed.")
        msg = ExecutionTrace()
        msg.event = "plan_complete"
        msg.data = json.dumps({"success": False, "reason": "no_pddl", "replan_count": 0})
        trace_pub.publish(msg)
        rclpy.shutdown()
        executor_thread.join()
        sys.exit(0)  # Exit 0 so launch reports "finished cleanly"; failure is in trace

    work_dir = Path(tempfile.mkdtemp())
    current_problem = work_dir / "problem.pddl"
    shutil.copy(problem_path, current_problem)
    plan_path = run_ff(domain_path, current_problem, "ff", work_dir)

    # If no initial plan (e.g. folder 9: faulty init makes problem unsolvable),
    # observe real state and replan once with corrected init
    if plan_path is None:
        config = observe_config(client)
        if config.get("motor_state") or config.get("spot_state"):
            next_problem = work_dir / "problem_retry_0.pddl"
            if update_problem_init_with_config(current_problem, config, next_problem):
                plan_path = run_ff(domain_path, next_problem, "ff", work_dir)
                if plan_path is not None:
                    current_problem = next_problem
                    print("No plan from faulty init; replanned with observed config and found plan.")
                    if trace_pub is not None:
                        msg = ExecutionTrace()
                        msg.event = "replan"
                        msg.data = json.dumps({
                            "trigger_action": "",
                            "trigger_reason": "no_initial_plan",
                            "config_snapshot": config,
                            "replan_count": 1,
                        })
                        trace_pub.publish(msg)

        if plan_path is None:
            try:
                shutil.rmtree(work_dir, ignore_errors=True)
            except Exception:
                pass
            print("ERROR: FF could not find initial plan (even after replan with observed state).")
            msg = ExecutionTrace()
            msg.event = "plan_complete"
            msg.data = json.dumps({"success": False, "reason": "no_plan", "replan_count": 0})
            trace_pub.publish(msg)
            rclpy.shutdown()
            executor_thread.join()
            sys.exit(0)  # Exit 0 so launch reports "finished cleanly"; failure is in trace

    plan = Plan.from_ff_file(str(plan_path))
    try:
        shutil.rmtree(work_dir, ignore_errors=True)
    except Exception:
        pass
    print(f"Initial plan: {[s.action for s in plan.steps()]}")

    success, replan_count = execute_plan(
        client, plan, pddl_folder, max_replans=3, trace_pub=trace_pub,
        post_skill_delay_sec=post_skill_delay,
    )

    if success:
        print("Mission completed successfully")
        if replan_count > 0:
            print(f"  (replanned {replan_count} time(s))")
    else:
        print("Mission failed")

    rclpy.shutdown()
    executor_thread.join()


if __name__ == "__main__":
    main()
