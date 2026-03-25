"""
Test mode: run the execution manager with a .ff plan and .json robust model
using a mock executor (no ROS/skillset required).
"""

from pathlib import Path
from typing import Dict, Optional, Union


def run_test_mode(
    plan_path: Union[str, Path],
    robust_model_path: Union[str, Path],
    initial_state: Optional[Dict[str, str]] = None,
    verbose: bool = True,
) -> bool:
    """
    Run the skill execution manager in test mode with a mock executor.

    Args:
        plan_path: Path to an FF planner output file (.ff).
        robust_model_path: Path to a robust model JSON file.
        initial_state: Initial configuration (motor_state, spot_state).
            Default: {"motor_state": "Off", "spot_state": "Sitting"}.
        verbose: If True, print progress and execution logs.

    Returns:
        True if the plan completed successfully, False if escalated.
    """
    from . import SkillExecutionManager, Plan, RobustModel
    from .mock_executor import MockExecutor

    plan_path = Path(plan_path)
    robust_model_path = Path(robust_model_path)

    if not robust_model_path.exists():
        raise FileNotFoundError(f"Robust model not found: {robust_model_path}")
    if not plan_path.exists():
        raise FileNotFoundError(f"Plan not found: {plan_path}")

    robust_model = RobustModel.from_file(str(robust_model_path))
    plan = Plan.from_ff_file(str(plan_path))

    state = dict(initial_state or {"motor_state": "Off", "spot_state": "Sitting"})
    observer = lambda: state
    executor = MockExecutor(state, log=verbose)

    def on_progress(p: dict) -> None:
        if verbose:
            print(
                f"  [Progress] step {p.get('current_step_index')}/{p.get('total_steps')} "
                f"status={p.get('status')!r} planned={p.get('planned_action')!r} "
                f"recovery_path_index={p.get('recovery_path_index')} "
                f"enablement={p.get('executed_enablement_path')}"
            )

    def on_escalation(action: str, config: dict, reason) -> None:
        if verbose:
            print(f"  [Escalation] action={action!r} config={config} reason={reason}")

    if verbose:
        print("Plan steps:", [f"{s.action}({s.parameters})" for s in plan.steps()])
        print("Initial state:", state)
        print("--- Run ---")

    manager = SkillExecutionManager(
        plan,
        robust_model,
        executor,
        observer,
        on_progress=on_progress,
        on_escalation=on_escalation,
    )
    completed = manager.run()

    if verbose:
        print("--- Done ---")
        print("Completed:", completed)
        print("Final state:", state)

    return completed
