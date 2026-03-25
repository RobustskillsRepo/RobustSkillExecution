#!/usr/bin/env python3
"""
CLI to run test campaign batches.
Usage:
  ros2 run test_campaign run_campaign.py --batch nominal
  ros2 run test_campaign run_campaign.py --batch faulty_pddl
  ros2 run test_campaign run_campaign.py --batch fault_injection
  ros2 run test_campaign run_campaign.py --batch skill_failure
  ros2 run test_campaign run_campaign.py --batch nominal --batch faulty_pddl
"""

import argparse
import os
import signal
import subprocess
import sys
import threading
import time
from datetime import datetime
from pathlib import Path

import psutil
import rclpy

from test_campaign.config import RunConfig, generate_batch_configs
from test_campaign.store import RunRecord, RunStore
from test_campaign.trace import TraceCollector

# Current run's (pid, pgid) for cleanup on Ctrl+C
_current_proc: tuple[int | None, int | None] = (None, None)


def _kill_process_tree(pid: int, pgid: int | None = None) -> None:
    """Kill process and all descendants. Uses process-group kill (launch uses start_new_session)
    so orphaned nodes are caught even if launch has exited. SIGTERM first, SIGKILL if needed."""
    if pgid is None:
        try:
            pgid = os.getpgid(pid)
        except OSError:
            pgid = None
    if pgid is not None:
        try:
            os.killpg(pgid, signal.SIGTERM)
        except OSError:
            pass
        time.sleep(1.0)
        try:
            os.killpg(pgid, signal.SIGKILL)
        except OSError:
            pass
    # Fallback: psutil for any stragglers
    try:
        parent = psutil.Process(pid)
    except psutil.NoSuchProcess:
        return
    children = parent.children(recursive=True)
    for child in children:
        try:
            child.terminate()
        except (psutil.NoSuchProcess, psutil.AccessDenied):
            pass
    psutil.wait_procs(children, timeout=2)
    for child in children:
        try:
            if child.is_running():
                child.kill()
        except (psutil.NoSuchProcess, psutil.AccessDenied):
            pass
    try:
        if parent.is_running():
            parent.terminate()
            parent.wait(timeout=2)
    except (psutil.NoSuchProcess, psutil.TimeoutExpired):
        pass
    try:
        if parent.is_running():
            parent.kill()
    except (psutil.NoSuchProcess, psutil.AccessDenied):
        pass


def _events_to_run_record(
    config: RunConfig,
    events: list,
    start_time: float,
    end_time: float,
    executor: str = "manager",
) -> RunRecord:
    """Build RunRecord from trace events."""
    run_id = str(datetime.utcnow().timestamp()).replace(".", "_")[:20]
    plan_initial = []
    skill_executions = []
    replans = []
    robust_skills_triggered = []
    outcome = {"success": False, "final_replan_count": 0}
    step_index = 0
    replan_index = 0
    current_enablement_path = []

    for ev in events:
        e = ev.get("event", "")
        d = ev.get("data", {})
        if e == "plan_start":
            replan_index = d.get("replan_count", 0)
            plan = d.get("plan", [])
            if replan_index == 0:
                plan_initial = plan
            elif replans:
                replans[-1]["plan_after_replan"] = plan
        elif e == "skill_start":
            if d.get("recovery"):
                robust_skills_triggered.append({
                    "skill": d.get("skill", ""),
                    "step_index": step_index,
                    "enablement_path": current_enablement_path,
                    "success": None,
                })
        elif e == "skill_end":
            skill = d.get("skill", "")
            success = d.get("success", False)
            if d.get("recovery"):
                if robust_skills_triggered:
                    robust_skills_triggered[-1]["success"] = success
            else:
                skill_executions.append({
                    "skill": skill,
                    "success": success,
                    "step_index": step_index,
                    "replan_index": replan_index,
                })
                if success and not d.get("after_recovery"):
                    step_index += 1
        elif e == "recovery_start":
            current_enablement_path = d.get("enablement_path", [])
        elif e == "replan":
            replans.append({
                "trigger_action": d.get("trigger_action", ""),
                "trigger_reason": d.get("trigger_reason", ""),
                "config_snapshot": d.get("config_snapshot", {}),
                "replan_count": d.get("replan_count", 0),
            })
        elif e == "plan_complete":
            outcome["success"] = d.get("success", False)
            outcome["reason"] = d.get("reason", "")
            outcome["final_replan_count"] = d.get("replan_count", 0)

    return RunRecord(
        run_id=run_id,
        batch_type=config.batch_type,
        config={
            "executor": executor,
            "pddl_model_folder": config.pddl_model_folder,
            "fault_injection_enabled": config.fault_injection_enabled,
            "fault_injection_params": config.fault_injection_params,
            "skill_failure_mode": config.skill_failure_mode,
            "skill_failure_target": config.skill_failure_target,
        },
        alterations=config.to_alterations(),
        plan_initial=plan_initial,
        skill_executions=skill_executions,
        replans=replans,
        robust_skills_triggered=robust_skills_triggered,
        outcome=outcome,
        timestamp=datetime.utcnow().isoformat() + "Z",
        duration_sec=end_time - start_time,
    )


def _print_run_summary(record: RunRecord) -> None:
    """Print deliberative feedback summary (plans, replans) after a run."""
    print(f"    [deliberative] plan_initial: {record.plan_initial}")
    for i, rp in enumerate(record.replans):
        after = rp.get("plan_after_replan", [])
        print(f"    [deliberative] replan {i+1}: trigger={rp.get('trigger_action')} "
              f"config={rp.get('config_snapshot')} -> plan: {after}")


def run_single(
    config: RunConfig,
    trace_topic: str,
    store: RunStore,
    timeout: float,
    reset_delay_sec: float = 2.0,
    startup_timeout_sec: float = 30.0,
    verbose: bool = False,
    executor: str = "manager",
) -> bool:
    """Launch stack with config, collect trace, store record. Returns True on success."""
    launch_file = "campaign_launch_naive.py" if executor == "naive" else "campaign_launch.py"
    params = [
        f"pddl_model_folder:={config.pddl_model_folder}",
        f"skill_failure_mode:={config.skill_failure_mode}",
        f"execution_trace_topic:={trace_topic}",
        f"fault_injection_enabled:={'true' if config.fault_injection_enabled else 'false'}",
    ]
    if config.skill_failure_target:
        params.append(f"skill_failure_target:={config.skill_failure_target}")
    if config.fault_injection_enabled:
        params.extend([
            f"inject_on_skill:={config.fault_injection_params.get('inject_on_skill', 'standup')}",
            f"inject_event:={config.fault_injection_params.get('inject_event', 'set_poweroff')}",
        ])
        if executor == "naive":
            params.append("post_skill_delay_sec:=0.25")  # Give fault injector time before next skill
            params.append("inject_delay_sec:=0.1")  # Delay injection so client gets SUCCESS first

    # Use None for stdout/stderr to inherit parent's (avoid pipe buffer deadlock when launch
    # writes output). With DEVNULL, launch errors are hidden; verbose uses inherited streams.
    # start_new_session=True puts launch and all child nodes in a new process group so we can
    # kill the entire tree on shutdown (avoids zombie nodes).
    # FASTDDS_BUILTIN_TRANSPORTS=UDPv4 avoids RTPS shared-memory port conflicts when running
    # multiple runs in sequence (zombie /dev/shm files from killed processes).
    out_err = None if verbose else subprocess.DEVNULL
    launch_env = os.environ.copy()
    launch_env["FASTDDS_BUILTIN_TRANSPORTS"] = "UDPv4"
    proc = subprocess.Popen(
        ["ros2", "launch", "test_campaign", launch_file] + params,
        stdout=out_err,
        stderr=out_err,
        env=launch_env,
        start_new_session=True,
    )
    try:
        pgid = os.getpgid(proc.pid) if proc.pid else None
    except OSError:
        pgid = None

    global _current_proc
    _current_proc = (proc.pid, pgid)
    rclpy.init()
    collector = TraceCollector(topic=trace_topic)
    start = time.time()
    spin_thread = threading.Thread(target=lambda: rclpy.spin(collector))
    spin_thread.start()

    try:
        # First wait for plan_start (detects startup hangs; mission needs ~15–20s to start)
        if not collector.wait_plan_start(timeout_sec=startup_timeout_sec):
            if verbose:
                print(f"    [timeout] No plan_start within {startup_timeout_sec}s; mission may have failed to start")
            got_complete = False
        else:
            got_complete = collector.wait_plan_complete(timeout_sec=max(60, timeout - (time.time() - start)))
    finally:
        rclpy.try_shutdown()
        spin_thread.join(timeout=2.0)
        collector.destroy_node()
        # Give mission ~2s to exit on its own (rclpy.shutdown, join) so launch reports "finished cleanly"
        time.sleep(2.0)
        if proc.pid is not None:
            _kill_process_tree(proc.pid, pgid)
        _current_proc = (None, None)

    # Reset between runs: the stack is fully terminated above. A delay ensures
    # clean shutdown (all nodes, topics, services, DDS) before the next run.
    time.sleep(reset_delay_sec)

    end = time.time()
    events = collector.get_events()
    record = _events_to_run_record(config, events, start, end, executor)
    store.append(record)
    if verbose:
        _print_run_summary(record)
    return got_complete


def _on_sigint(signum, frame):
    """Kill current launch process tree on Ctrl+C, then re-raise."""
    global _current_proc
    pid, pgid = _current_proc
    if pid is not None:
        _kill_process_tree(pid, pgid)
        _current_proc = (None, None)
    raise KeyboardInterrupt()


def main():
    # Avoid RTPS shared-memory port conflicts when running multiple runs in sequence
    os.environ.setdefault("FASTDDS_BUILTIN_TRANSPORTS", "UDPv4")
    signal.signal(signal.SIGINT, _on_sigint)

    parser = argparse.ArgumentParser(description="Run test campaign batches")
    parser.add_argument("--batch", action="append", required=True,
        choices=["nominal", "faulty_pddl", "fault_injection", "skill_failure"],
        help="Batch type(s) to run")
    parser.add_argument("--executor", choices=["manager", "naive"], default="manager",
        help="manager: skill_execution_manager + deliberative; naive: quadruped_mission (replan on failure)")
    parser.add_argument("--trace-topic", default="execution_trace")
    parser.add_argument("--store", type=Path, default=None)
    parser.add_argument("--timeout", type=float, default=300.0)
    parser.add_argument("--reset-delay", type=float, default=3.0,
        help="Seconds to wait between runs for clean stack shutdown (DDS, nodes)")
    parser.add_argument("--startup-timeout", type=float, default=30.0,
        help="Seconds to wait for plan_start before aborting (detects startup hangs)")
    parser.add_argument("--verbose", "-v", action="store_true",
        help="Show launch output (nodes, errors); otherwise suppressed")
    args = parser.parse_args()

    store = RunStore(args.store)
    trace_topic = args.trace_topic

    for batch_type in args.batch:
        configs = generate_batch_configs(batch_type)
        print(f"Running batch {batch_type} ({len(configs)} runs) [executor={args.executor}]")
        for i, config in enumerate(configs):
            print(f"  Run {i+1}/{len(configs)}: {config.to_alterations() or 'nominal'}")
            run_single(config, trace_topic, store, args.timeout, args.reset_delay,
                args.startup_timeout, args.verbose, args.executor)

    print(f"Store: {store.path}")

if __name__ == "__main__":
    main()
