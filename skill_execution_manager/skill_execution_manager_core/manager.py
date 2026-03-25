"""
Main loop of the skill execution manager (algorithm from main.tex).
Observe -> classify (Nom / Unrec / Rec, Phi_sys) -> execute enablement path if Rec -> execute planned skill -> advance or escalate.
Phi_sys: system property; s violates Phi_sys → Impossible.
"""

import time
from typing import Any, Callable, Dict, List, Optional, Set

from .escalation import EscalationReason
from .executor import SkillExecutor
from .plan import Plan, PlanStep
from .robust_model import RobustModel, SkillPartition
from .state_checker import StateChecker, state_from_resources


def _get_observer_state(observe: Callable[[], Any]) -> Dict[str, str]:
    """Get current configuration as state dict (resource -> value)."""
    raw = observe()
    if isinstance(raw, dict):
        return dict(raw)
    return state_from_resources(raw)


def _format_state(state: Dict[str, str]) -> str:
    """Format state dict for display (exclude internal keys like at_location if desired)."""
    parts = [f"{k}={v}" for k, v in sorted(state.items()) if k != "at_location" or v]
    return ", ".join(parts) if parts else "(empty)"


def _state_key(state: Dict[str, str]) -> tuple:
    """Hashable key for state (for Tried set). Uses resource-relevant keys only."""
    return tuple(sorted((k, v) for k, v in state.items() if k != "at_location"))


# Sentinel for "we already tried retrying sigma from nominal" (avoids infinite retry loop)
_NOMINAL_TRIED_KEY: tuple = ("__nominal__",)


def _achieved_predicates_from_plan(plan: Plan, failed_at_index: int) -> Dict[str, Any]:
    """
    Infer achieved predicates (picture_taken_at, charged) from plan steps 0..failed_at_index-1.
    Returns dict to merge into config for update_problem_init_with_config.
    """
    achieved: Dict[str, Any] = {"picture_taken_at": [], "charged": False}
    for idx in range(min(failed_at_index, len(plan))):
        step = plan[idx]
        action = step.action.lower()
        params = step.parameters or []
        if action == "take_picture" and params:
            loc = str(params[0]).strip().upper()
            if loc not in achieved["picture_taken_at"]:
                achieved["picture_taken_at"].append(loc)
        elif action == "recharge":
            achieved["charged"] = True
    return achieved


class SkillExecutionManager:
    """
    Execution manager with enablement paths.
    Requires: plan, robust model, executor (backed by skillset client), state observer.
    Ensures: stream of execution queries and plan progress; on failure, escalation to deliberative layer.
    """

    def __init__(
        self,
        plan: Plan,
        robust_model: RobustModel,
        executor: SkillExecutor,
        observe: Callable[[], Any],
        on_progress: Optional[Callable[[Dict[str, Any]], None]] = None,
        on_escalation: Optional[Callable[[str, Dict[str, str], EscalationReason], None]] = None,
        on_feedback: Optional[Callable[[str], None]] = None,
        on_trace: Optional[Callable[[str, Dict], None]] = None,
    ):
        self._plan = plan
        self._robust = robust_model
        self._executor = executor
        self._observe = observe
        self._on_progress = on_progress or (lambda _: None)
        self._on_escalation = on_escalation or (lambda *_: None)
        self._on_feedback = on_feedback or (lambda _: None)
        self._on_trace = on_trace or (lambda _e, _d: None)

    def _feedback(self, msg: str) -> None:
        """Emit feedback to the user."""
        self._on_feedback(msg)

    def _emit_progress(
        self,
        current_step: int,
        total_steps: int,
        status: str,
        planned_action: Optional[str] = None,
        recovery_path_index: Optional[int] = None,
        executed_enablement_path: Optional[List[str]] = None,
    ) -> None:
        self._on_progress({
            "current_step_index": current_step,
            "total_steps": total_steps,
            "status": status,
            "planned_action": planned_action,
            "recovery_path_index": recovery_path_index,
            "executed_enablement_path": executed_enablement_path,
        })

    def _notify_deliberative(self, action: str, config: Dict[str, str], reason: EscalationReason) -> None:
        self._on_escalation(action, config, reason)

    def _trace(self, event: str, data: Dict) -> None:
        self._on_trace(event, data)

    def run(self) -> bool:
        """
        Run the plan. Returns True if PlanCompleted, False if escalated.
        Algorithm: try enablement paths on failure instead of escalating; avoid infinite loops
        by tracking configurations that already triggered enablement (Tried set, reset per step).
        """
        k = len(self._plan)
        i = 0
        plan_actions = [s.action for s in self._plan]
        self._trace("plan_start", {"plan": plan_actions, "total_steps": k})

        while i < k:
            step: PlanStep = self._plan[i]
            sigma = step.action
            params = step.parameters
            step_label = f"Step {i + 1}/{k}: {sigma}"

            part = self._robust.get_partition(sigma)
            if part is None:
                # No robust model: execute directly
                self._feedback(f"{step_label} | No robust model → executing directly")
                self._emit_progress(i, k, "running", planned_action=sigma)
                self._trace("skill_start", {"skill": sigma, "step_index": i, "params": params})
                ok = self._executor.execute_and_wait(sigma, params)
                self._trace("skill_end", {"skill": sigma, "step_index": i, "success": ok})
                if not ok:
                    time.sleep(0.1)  # Let quadruped publish status for accurate escalation config
                    s = _get_observer_state(self._observe)
                    s.update(_achieved_predicates_from_plan(self._plan, i))
                    self._feedback(f"{step_label} | Skill execution failed → escalate")
                    self._trace("escalation", {"action": sigma, "reason": "SKILL_FAILED", "config": s})
                    self._notify_deliberative(sigma, s, EscalationReason.SKILL_FAILED)
                    return False
                i += 1
                continue

            nominal_smt = part.nominal_smt
            unrecoverable_smt = part.unrecoverable_smt
            rec_list = part.recoverable
            phi_sys = self._robust.system_property_smt

            s_dict = _get_observer_state(self._observe)
            checker = StateChecker(s_dict)
            state_str = _format_state(s_dict)

            # Φ_sys: s violates system property → Impossible
            if phi_sys and phi_sys.strip().lower() not in ("", "true") and not checker.satisfies(phi_sys):
                s_dict.update(_achieved_predicates_from_plan(self._plan, i))
                self._feedback(f"{step_label} | Observed: {state_str} | Impossible → escalate")
                self._trace("escalation", {"action": sigma, "reason": "IMPOSSIBLE", "config": s_dict})
                self._notify_deliberative(sigma, s_dict, EscalationReason.IMPOSSIBLE)
                return False

            # Unrec_σ
            if checker.satisfies(unrecoverable_smt):
                s_dict.update(_achieved_predicates_from_plan(self._plan, i))
                self._feedback(f"{step_label} | Observed: {state_str} | Unrecoverable → escalate")
                self._trace("escalation", {"action": sigma, "reason": "UNRECOVERABLE", "config": s_dict})
                self._notify_deliberative(sigma, s_dict, EscalationReason.UNRECOVERABLE)
                return False

            step_done = False
            if checker.satisfies(nominal_smt):
                self._feedback(f"{step_label} | Observed: {state_str} | Nominal → executing")
                self._emit_progress(i, k, "running", planned_action=sigma)
                self._trace("skill_start", {"skill": sigma, "step_index": i, "params": params})
                ok = self._executor.execute_and_wait(sigma, params)
                self._trace("skill_end", {"skill": sigma, "step_index": i, "success": ok})
                if ok:
                    time.sleep(0.15)  # Let quadruped publish updated SkillsetStatus before next observe
                    i += 1
                    step_done = True
                # If fail: fall through to recovery loop (do not escalate yet)

            if not step_done:
                # Recovery loop: try nominal retry first (if still in nominal), then enablement paths.
                # Tried set: nominal retry + configurations that already triggered enablement.
                recoverable = len(rec_list) > 0
                tried: Set[tuple] = set()

                # Try nominal retry first (even when no recovery paths)
                time.sleep(0.1)  # Let quadruped publish status for accurate observe
                s_retry = _get_observer_state(self._observe)
                checker_retry = StateChecker(s_retry)
                if checker_retry.satisfies(nominal_smt):
                    tried.add(_NOMINAL_TRIED_KEY)
                    self._feedback(f"{step_label} | Still nominal → retrying {sigma} (params={params})")
                    self._emit_progress(i, k, "running", planned_action=sigma)
                    self._trace("skill_start", {"skill": sigma, "step_index": i, "params": params, "retry": True})
                    ok = self._executor.execute_and_wait(sigma, params)
                    self._trace("skill_end", {"skill": sigma, "step_index": i, "success": ok, "retry": True})
                    if ok:
                        time.sleep(0.15)
                        i += 1
                        step_done = True  # Advance to next step

                if not step_done and not recoverable:
                    time.sleep(0.1)  # Let quadruped publish status for accurate escalation config
                    s_now = _get_observer_state(self._observe)
                    s_now.update(_achieved_predicates_from_plan(self._plan, i))
                    self._feedback(f"{step_label} | No recovery path available → escalate")
                    self._trace("escalation", {"action": sigma, "reason": "SKILL_FAILED", "config": s_now})
                    self._notify_deliberative(sigma, s_now, EscalationReason.SKILL_FAILED)
                    return False

                while recoverable and not step_done:
                    s_dict = _get_observer_state(self._observe)
                    checker = StateChecker(s_dict)
                    state_str = _format_state(s_dict)
                    s_key = _state_key(s_dict)

                    # If still in nominal and we haven't tried nominal retry yet: retry sigma (with params)
                    if checker.satisfies(nominal_smt) and _NOMINAL_TRIED_KEY not in tried:
                        tried.add(_NOMINAL_TRIED_KEY)
                        self._feedback(f"{step_label} | Still nominal → retrying {sigma} (params={params})")
                        self._emit_progress(i, k, "running", planned_action=sigma)
                        self._trace("skill_start", {"skill": sigma, "step_index": i, "params": params, "retry": True})
                        ok = self._executor.execute_and_wait(sigma, params)
                        self._trace("skill_end", {"skill": sigma, "step_index": i, "success": ok, "retry": True})
                        if ok:
                            time.sleep(0.15)
                            i += 1
                            break
                        continue  # Failed again, try recovery paths

                    # Find P in Rec s.t. s in P and s not in Tried
                    chosen_idx: Optional[int] = None
                    chosen_path: Optional[List[str]] = None
                    for idx, rec in enumerate(rec_list):
                        region_smt = rec.get("region_smt", "false")
                        if checker.satisfies(region_smt) and s_key not in tried:
                            chosen_path = rec.get("enablement_path", [])
                            chosen_idx = idx
                            break

                    if chosen_path is None:
                        time.sleep(0.1)  # Let quadruped publish status for accurate escalation config
                        s_now = _get_observer_state(self._observe)
                        s_now.update(_achieved_predicates_from_plan(self._plan, i))
                        self._feedback(
                            f"{step_label} | Observed: {state_str} | No untried recovery region → escalate"
                        )
                        self._trace("escalation", {"action": sigma, "reason": "RECOVERY_FAILED", "config": s_now})
                        self._notify_deliberative(sigma, s_now, EscalationReason.RECOVERY_FAILED)
                        return False

                    tried.add(s_key)
                    path_str = " → ".join(chosen_path)
                    self._trace("recovery_start", {"step_index": i, "planned_action": sigma, "enablement_path": chosen_path})
                    self._feedback(
                        f"{step_label} | Observed: {state_str} | Non-nominal → recovery path [{path_str}]"
                    )
                    self._emit_progress(
                        i, k, "recovery",
                        planned_action=sigma,
                        recovery_path_index=chosen_idx,
                        executed_enablement_path=chosen_path,
                    )

                    # Execute enablement path
                    enablement_failed = False
                    for tau in chosen_path:
                        self._feedback(f"{step_label} | Executing recovery: {tau}")
                        self._trace("skill_start", {"skill": tau, "step_index": i, "recovery": True})
                        ok = self._executor.execute_and_wait(tau, [])
                        self._trace("skill_end", {"skill": tau, "step_index": i, "success": ok, "recovery": True})
                        if not ok:
                            enablement_failed = True
                            break  # Keep trying to recover (loop again with new state)

                    self._trace("recovery_end", {"step_index": i, "enablement_path": chosen_path, "success": not enablement_failed})
                    if enablement_failed:
                        continue  # Re-observe, maybe we're in a different region now

                    s_after = _get_observer_state(self._observe)
                    checker_after = StateChecker(s_after)
                    if not checker_after.satisfies(nominal_smt):
                        state_after_str = _format_state(s_after)
                        self._feedback(
                            f"{step_label} | After recovery: {state_after_str} | Still non-nominal, retrying..."
                        )
                        continue  # Try another recovery path (state may have changed)

                    # GetState() in Nom_σ: Execute(σ) with original params (e.g. go_to target)
                    state_after_str = _format_state(s_after)
                    self._feedback(f"{step_label} | After recovery: {state_after_str} | Nominal → executing {sigma}")
                    self._emit_progress(i, k, "running", planned_action=sigma)
                    self._trace("skill_start", {"skill": sigma, "step_index": i, "params": params, "after_recovery": True})
                    ok = self._executor.execute_and_wait(sigma, params)
                    self._trace("skill_end", {"skill": sigma, "step_index": i, "success": ok, "after_recovery": True})
                    if ok:
                        time.sleep(0.15)  # Let quadruped publish updated SkillsetStatus
                        recoverable = False
                        i += 1
                    else:
                        tried.add(_NOMINAL_TRIED_KEY)  # Tried sigma from nominal (after recovery), failed
                        continue

        self._feedback(f"Plan completed ({k}/{k})")
        self._trace("plan_complete", {"success": True, "total_steps": k})
        self._emit_progress(k, k, "completed")
        return True
