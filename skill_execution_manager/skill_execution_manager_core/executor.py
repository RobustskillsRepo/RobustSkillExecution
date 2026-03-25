"""
Execute skills via the quadruped_example_skillset_client.
Execute(sigma) = start skill, WaitResult(sigma) = wait and return success/fail.
"""

from typing import Any, List, Optional

# Optional: only import when running under ROS 2 with the client installed
try:
    from quadruped_example_skillset_interfaces.msg import SkillGoToInput
except ImportError:
    SkillGoToInput = None  # type: ignore


# Result code for success (from Skill*Response.msg)
SUCCESS = 0


class SkillExecutor:
    """Dispatches skill execution to the skillset client and waits for result."""

    def __init__(self, client: Any):
        """
        Args:
            client: QuadrupedExampleSkillsetClient instance (from quadruped_example_skillset_client).
        """
        self._client = client
        self._last_id: Optional[str] = None
        self._last_skill_name: Optional[str] = None

    def execute(self, skill_name: str, parameters: Optional[List[Any]] = None) -> str:
        """
        Emit Execute(sigma): start the skill. Returns request id for wait_result.
        skill_name: e.g. 'init_power', 'standup', 'go_to'
        parameters: for go_to, e.g. ['L2'] -> input.target = 'L2'
        """
        parameters = parameters or []
        skill = getattr(self._client.skills, skill_name, None)
        if skill is None:
            raise ValueError(f"Unknown skill: {skill_name}")

        if skill_name == "go_to":
            if SkillGoToInput is not None:
                inp = SkillGoToInput()
                inp.target.data = str(parameters[0]) if parameters else ""
                self._last_id = skill.start(inp)
            else:
                self._last_id = skill.start()
        else:
            self._last_id = skill.start()

        self._last_skill_name = skill_name
        return self._last_id

    def wait_result(self, skill_name: str, request_id: Optional[str] = None) -> bool:
        """
        Wait for skill result. Returns True iff result is SUCCESS.
        """
        rid = request_id or self._last_id
        skill_name = skill_name or self._last_skill_name
        if not rid or not skill_name:
            return False
        skill = getattr(self._client.skills, skill_name, None)
        if skill is None:
            return False
        response = skill.wait_result(rid)
        result_code = getattr(response, "result", -1)
        ok = result_code == SUCCESS
        if not ok and hasattr(self._client, "node") and self._client.node is not None:
            try:
                self._client.node.get_logger().warn(
                    f"Skill {skill_name} returned result={result_code} (SUCCESS=0)"
                )
            except Exception:
                pass
        return ok

    def execute_and_wait(self, skill_name: str, parameters: Optional[List[Any]] = None) -> bool:
        """Execute(sigma) then WaitResult(sigma). Returns True on success."""
        self.execute(skill_name, parameters)
        return self.wait_result(skill_name)
