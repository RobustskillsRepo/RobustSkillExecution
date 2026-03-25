"""Run record storage (JSONL)."""

import json
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Dict, List, Optional

from .config import RunConfig


@dataclass
class RunRecord:
    """Collected data for a single run."""

    run_id: str
    batch_type: str
    config: Dict[str, Any]
    alterations: List[Dict[str, Any]]
    plan_initial: List[str]
    skill_executions: List[Dict[str, Any]]
    replans: List[Dict[str, Any]]
    robust_skills_triggered: List[Dict[str, Any]]
    outcome: Dict[str, Any]
    timestamp: str
    duration_sec: float

    def to_dict(self) -> Dict[str, Any]:
        return {
            "run_id": self.run_id,
            "batch_type": self.batch_type,
            "config": self.config,
            "alterations": self.alterations,
            "plan_initial": self.plan_initial,
            "skill_executions": self.skill_executions,
            "replans": self.replans,
            "robust_skills_triggered": self.robust_skills_triggered,
            "outcome": self.outcome,
            "timestamp": self.timestamp,
            "duration_sec": self.duration_sec,
        }


class RunStore:
    """Persist RunRecords to JSONL file."""

    def __init__(self, path: Optional[Path] = None):
        if path is None:
            base = Path.home() / ".ros" / "test_campaign"
            base.mkdir(parents=True, exist_ok=True)
            path = base / "runs.jsonl"
        self._path = Path(path)
        self._path.parent.mkdir(parents=True, exist_ok=True)

    @property
    def path(self) -> Path:
        return self._path

    def append(self, record: RunRecord) -> None:
        """Append a run record to the store."""
        with open(self._path, "a") as f:
            f.write(json.dumps(record.to_dict()) + "\n")

    def load_all(self) -> List[Dict[str, Any]]:
        """Load all records from the store."""
        if not self._path.exists():
            return []
        records = []
        with open(self._path) as f:
            for line in f:
                line = line.strip()
                if not line:
                    continue
                try:
                    records.append(json.loads(line))
                except json.JSONDecodeError:
                    pass
        return records
