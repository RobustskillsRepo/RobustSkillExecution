"""Run and batch configuration for test campaigns."""

from dataclasses import dataclass, field
from typing import Any, Dict, List


@dataclass
class RunConfig:
    """Per-run parameters for a test campaign execution."""

    batch_type: str  # "nominal" | "faulty_pddl" | "fault_injection" | "skill_failure"
    pddl_model_folder: int = 0
    fault_injection_enabled: bool = False
    fault_injection_params: Dict[str, Any] = field(default_factory=dict)
    skill_failure_mode: str = "none"  # "none" | "once" | "always"
    skill_failure_target: str = ""

    def to_alterations(self) -> List[Dict[str, Any]]:
        """List of alterations applied in this run for recording."""
        alts = []
        if self.batch_type == "faulty_pddl" and self.pddl_model_folder != 0:
            alts.append({"type": "pddl_folder", "value": self.pddl_model_folder})
        if self.batch_type == "fault_injection" and self.fault_injection_enabled:
            alts.append({"type": "fault_injection", "value": self.fault_injection_params})
        if self.batch_type == "skill_failure" and self.skill_failure_mode != "none":
            alts.append({
                "type": "skill_failure",
                "value": f"{self.skill_failure_target}:{self.skill_failure_mode}",
            })
        return alts

    def to_ros_params(self) -> Dict[str, Any]:
        """ROS2 parameters for launching the stack with this config."""
        params = {
            "pddl_model_folder": self.pddl_model_folder,
            "skill_failure_mode": self.skill_failure_mode,
            "skill_failure_target": self.skill_failure_target,
        }
        return params


def generate_batch_configs(
    batch_type: str,
    pddl_folders: List[int] | None = None,
    fault_injection_variants: List[Dict[str, Any]] | None = None,
    skill_failure_variants: List[tuple] | None = None,
) -> List[RunConfig]:
    """
    Generate RunConfig list for a batch type.
    - nominal: single run with default config
    - faulty_pddl: one run per pddl_folder (default 1..11)
    - fault_injection: one run per variant (inject_on_skill, inject_event, etc.)
    - skill_failure: one run per target with mode "once" (always-fail would block the plan)
    """
    configs: List[RunConfig] = []
    if batch_type == "nominal":
        configs.append(RunConfig(batch_type="nominal"))
    elif batch_type == "faulty_pddl":
        folders = pddl_folders or list(range(1, 12))
        for f in folders:
            configs.append(RunConfig(
                batch_type="faulty_pddl",
                pddl_model_folder=f,
            ))
    elif batch_type == "fault_injection":
        # Full batch: for each target skill × each resource, inject a fault (change resource value)
        # right after the skill completes. 7 skills × 2 resources = 14 runs.
        skills = [
            "init_power", "safe_poweroff", "standup", "sitdown",
            "go_to", "recharge", "take_picture",
        ]
        # resource -> event that changes it (disruptive fault)
        resource_events = [
            ("motor_state", "set_poweroff"),   # On -> Off
            ("spot_state", "set_sitting"),     # Standing -> Sitting
        ]
        variants = fault_injection_variants or [
            {"inject_on_skill": skill, "inject_event": event, "inject_resource": resource}
            for skill in skills
            for resource, event in resource_events
        ]
        for v in variants:
            configs.append(RunConfig(
                batch_type="fault_injection",
                fault_injection_enabled=True,
                fault_injection_params=v,
            ))
    elif batch_type == "skill_failure":
        skills = [
            "init_power", "safe_poweroff", "standup", "sitdown",
            "go_to", "recharge", "take_picture",
        ]
        variants = skill_failure_variants or [
            (s, "once") for s in skills
        ]
        for target, mode in variants:
            configs.append(RunConfig(
                batch_type="skill_failure",
                skill_failure_mode=mode,
                skill_failure_target=target,
            ))
    return configs


@dataclass
class BatchConfig:
    """Configuration for a batch of runs."""

    batch_type: str
    run_configs: List[RunConfig]
