"""Load and represent a PDDL plan as an ordered list of steps (action + parameters)."""

import json
import re
from pathlib import Path
from typing import Any, Dict, List, Optional


def _normalize_action_name(name: str) -> str:
    """Map FF/PDDL action name to skillset name (lowercase with underscores)."""
    s = name.strip().lower().replace("-", "_")
    return s


class PlanStep:
    def __init__(self, action: str, parameters: Optional[List[Any]] = None):
        self.action = action
        self.parameters = list(parameters or [])

    def __repr__(self):
        return f"PlanStep({self.action!r}, {self.parameters!r})"


class Plan:
    def __init__(self, steps: List[PlanStep]):
        self._steps = list(steps)

    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> "Plan":
        steps = []
        for s in data.get("steps", []):
            steps.append(PlanStep(
                action=s.get("action", ""),
                parameters=s.get("parameters", []),
            ))
        return cls(steps)

    @classmethod
    def from_file(cls, path: str) -> "Plan":
        with open(path, "r") as f:
            return cls.from_dict(json.load(f))

    @classmethod
    def from_ff_file(cls, path: str) -> "Plan":
        """Parse an FF planner output file (plan.ff). Lines like 'step 0: INIT_POWER' or '       2: GO_TO L2 L0'."""
        steps = []
        step_re = re.compile(r"step\s+(\d+):\s*(.+)", re.IGNORECASE)
        cont_re = re.compile(r"\s+(\d+):\s*(.+)")
        with open(path, "r") as f:
            for line in f:
                line = line.rstrip()
                m = step_re.match(line) or cont_re.match(line)
                if not m:
                    continue
                rest = m.group(2).strip()
                parts = rest.split()
                if not parts:
                    continue
                action = _normalize_action_name(parts[0])
                parameters = parts[1:] if len(parts) > 1 else []
                steps.append(PlanStep(action=action, parameters=parameters))
        return cls(steps)

    def __len__(self) -> int:
        return len(self._steps)

    def __getitem__(self, i: int) -> PlanStep:
        return self._steps[i]

    def steps(self) -> List[PlanStep]:
        return list(self._steps)
