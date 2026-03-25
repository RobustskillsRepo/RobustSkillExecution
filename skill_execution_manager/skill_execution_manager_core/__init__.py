"""
Skill execution manager: executes a plan using robust skill models.
Observes configuration, classifies into Nom/Rec/Unrec (Φ_sys for Impossible), runs nominal or enablement path, escalates on failure.
"""

from .manager import SkillExecutionManager
from .robust_model import RobustModel
from .plan import Plan
from .state_checker import StateChecker, state_from_resources
from .executor import SkillExecutor
from .escalation import EscalationReason
from .test_mode import run_test_mode

__all__ = [
    "SkillExecutionManager",
    "RobustModel",
    "Plan",
    "StateChecker",
    "SkillExecutor",
    "EscalationReason",
    "state_from_resources",
    "run_test_mode",
]
