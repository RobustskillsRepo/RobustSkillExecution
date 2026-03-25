"""
Mock executor and state simulator for testing the manager without ROS/skillset.
Applies skill effects to a mutable state dict so the observer sees updated configuration.
"""

from typing import Any, Dict, List, Optional


# Skill effects: (motor_state, spot_state) after executing skill from current state.
# Only skills that change state are listed; others leave state unchanged.
EFFECTS: Dict[str, Dict[str, str]] = {
    "init_power": {"motor_state": "On", "spot_state": "Sitting"},
    "safe_poweroff": {"motor_state": "Off", "spot_state": "Sitting"},
    "standup": {"motor_state": "On", "spot_state": "Standing"},
    "sitdown": {"motor_state": "On", "spot_state": "Sitting"},
    # go_to, take_picture, recharge: no resource change in our model
}


class MockExecutor:
    """
    Executor that simulates skill execution by updating a shared state dict.
    Always returns success (True). Use for dry-run tests without the skillset client.
    """

    def __init__(self, state: Dict[str, str], log: bool = True):
        self._state = state
        self._log = log
        self._last_id: Optional[str] = None
        self._last_skill_name: Optional[str] = None

    def execute(self, skill_name: str, parameters: Optional[List[Any]] = None) -> str:
        parameters = parameters or []
        if self._log:
            print(f"  [MockExecutor] Execute: {skill_name!r} params={parameters}")
        effect = EFFECTS.get(skill_name)
        if effect:
            self._state.update(effect)
        self._last_id = f"mock-{skill_name}-0"
        self._last_skill_name = skill_name
        return self._last_id

    def wait_result(self, skill_name: str, request_id: Optional[str] = None) -> bool:
        if self._log:
            print(f"  [MockExecutor] WaitResult: {skill_name!r} -> success")
        return True

    def execute_and_wait(self, skill_name: str, parameters: Optional[List[Any]] = None) -> bool:
        self.execute(skill_name, parameters)
        return self.wait_result(skill_name)
