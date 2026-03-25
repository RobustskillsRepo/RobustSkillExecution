"""Escalation reasons for NotifyDeliberative (algorithm in main.tex)."""

from enum import Enum


class EscalationReason(str, Enum):
    """Reasons for escalating to the deliberative layer."""

    IMPOSSIBLE = "Impossible"  # s in Phi_sys (system property violated)
    UNRECOVERABLE = "Unrecoverable"  # s in Unrec_sigma (no enablement path)
    RECOVERY_FAILED = "RecoveryFailed"  # exhausted recovery (no P with s in P and s not in Tried)
    ENABLEMENT_FAILURE = "EnablementFailure"  # skill in enablement path failed (legacy)
    ENAB_FAIL_NO_EFFECT = "EnabFail_NoEffect"  # after enablement path, s not in Nom_sigma
    SKILL_FAILED = "SkillFailed"  # planned skill execution failed, no recoverable region
