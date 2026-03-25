"""Campaign runner: orchestrates batches, launches stack, collects traces."""

import json
import subprocess
import threading
import time
from datetime import datetime
from pathlib import Path
from typing import Any, Dict, List, Optional

from .config import RunConfig, generate_batch_configs
from .store import RunRecord, RunStore
from .trace import TraceCollector


def _events_to_run_record(
    config: RunConfig,
    events: List[Dict[str, Any]],
    start_time: float,
    end_time: float,
) -> RunRecord:
    """Build RunRecord from trace events."""
    run_id = str(datetime.utcnow().timestamp()).replace(".", "_")[:20]
    plan_initial: List[str] = []
    skill_executions: List[Dict[str, Any]] = []
    replans: List[Dict[str, Any]] = []
    robust_skills_triggered: List[Dict[str, Any]] = []
    outcome: Dict[str, Any] = {"success": False, "final_replan_count": 0}

    step_index = 0
    replan_index = 0
    in_recovery = False
    current_enablement_path: List[str] = []

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
            skill = d.get("skill", "")
            if d.get("recovery"):
                robust_skills_triggered.append({
                    "skill": skill,
                    "step_index": step_index,
                    "enablement_path": current_enablement_path,
                    "success": None,
                })
            elif not d.get("after_recovery"):
                pass  # will get success from skill_end
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
            in_recovery = True
            current_enablement_path = d.get("enablement_path", [])
        elif e == "recovery_end":
            in_recovery = False
        elif e == "replan":
            replans.append({
                "trigger_action": d.get("trigger_action", ""),
                "trigger_reason": d.get("trigger_reason", ""),
                "config_snapshot": d.get("config_snapshot", {}),
                "replan_count": d.get("replan_count", 0),
            })
        elif e == "escalation":
            outcome["success"] = False
            outcome["reason"] = d.get("reason", "unknown")
        elif e == "plan_complete":
            outcome["success"] = d.get("success", False)
            outcome["reason"] = d.get("reason", "")
            outcome["final_replan_count"] = d.get("replan_count", 0)

    return RunRecord(
        run_id=run_id,
        batch_type=config.batch_type,
        config={
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


class CampaignRunner:
    """Orchestrates test campaign: launch stack, collect traces, store records."""

    def __init__(
        self,
        trace_topic: str = "execution_trace",
        store_path: Optional[Path] = None,
        run_timeout_sec: float = 300.0,
    ):
        self._trace_topic = trace_topic
        self._store = RunStore(store_path)
        self._run_timeout = run_timeout_sec

    def run_single(self, config: RunConfig) -> Optional[RunRecord]:
        """
        Run a single configuration. Launches the stack via campaign_launch,
        waits for plan_complete, builds RunRecord, returns it.
        """
        # Launch is handled externally - the runner assumes the stack is
        # already launched with the right params, or we need to launch it.
        # Per the plan: "Launch stack with campaign_launch.py passing run config"
        # So we need to spawn the launch and then run the trace collector.
        # For simplicity: run_campaign.py will do the launch + trace in one process.
        # The CampaignRunner provides the logic; run_campaign.py does the orchestration.
        return None  # Placeholder - actual launch in run_campaign.py
