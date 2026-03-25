"""
Load and expose robust skill model (JSON).
Per-skill: Nom_σ, Rec_σ (list of (region_smt, enablement_path)), Unrec_σ.
Phi_sys (system_property_smt) is global: s violates it → Impossible (no per-skill Imp).
"""

import json
from pathlib import Path
from typing import Any, Dict, List, Optional


class SkillPartition:
    """Partition regions and enablement map for one skill."""

    __slots__ = ("nominal_smt", "unrecoverable_smt", "recoverable")

    def __init__(
        self,
        nominal_smt: str,
        unrecoverable_smt: Optional[str] = None,
        recoverable: Optional[List[Dict[str, Any]]] = None,
    ):
        self.nominal_smt = nominal_smt or "false"
        self.unrecoverable_smt = unrecoverable_smt or "false"
        self.recoverable = list(recoverable or [])

    def get_enablement_path_for_region_smt(self, region_smt: str) -> Optional[List[str]]:
        """Return enablement_path for the recoverable entry with given region_smt."""
        for entry in self.recoverable:
            if entry.get("region_smt") == region_smt:
                return list(entry.get("enablement_path", []))
        return None


class RobustModel:
    """Robust model M_R = (P, E): partitions and enablement paths per skill."""

    def __init__(self, data: Dict[str, Any]):
        self._data = data
        self._resources = list(data.get("resources", []))
        self._system_property_smt = data.get("system_property_smt", "true")
        self._skills: Dict[str, SkillPartition] = {}
        for name, skill_data in data.get("skills", {}).items():
            self._skills[name] = SkillPartition(
                nominal_smt=skill_data.get("nominal_smt", "false"),
                unrecoverable_smt=skill_data.get("unrecoverable_smt"),
                recoverable=skill_data.get("recoverable", []),
            )

    @classmethod
    def from_file(cls, path: str) -> "RobustModel":
        with open(path, "r") as f:
            return cls(json.load(f))

    def get_partition(self, action_name: str) -> Optional[SkillPartition]:
        return self._skills.get(action_name)

    def has_skill(self, action_name: str) -> bool:
        return action_name in self._skills

    def skill_names(self):
        return list(self._skills.keys())

    @property
    def resources(self):
        return list(self._resources)

    @property
    def system_property_smt(self) -> str:
        return self._system_property_smt
