"""
Evaluate SMT-LIB2 Boolean formulas on a concrete configuration.

Formulas come from skill_reinforcement robust models (QE/simplify pipeline).
State s: dict resource_name -> value (e.g. {"motor_state": "On", "spot_state": "Sitting"}).
Atom resource__value is true iff state[resource] == value.

Supported connectives (prefix form):
  (and φ₁ φ₂ … φₙ)
  (or φ₁ φ₂ … φₙ)
  (not φ)
  (=> φ ψ)   implication
  (= φ ψ)    iff (equivalence)

Literals: true, false
"""

import re
from typing import Any, Dict


def _tokenize(s: str) -> list:
    """Tokenize SMT formula: parentheses and atoms."""
    s = s.strip()
    tokens = []
    i = 0
    while i < len(s):
        if s[i] in "()":
            tokens.append(s[i])
            i += 1
        elif s[i].isspace():
            i += 1
        else:
            j = i
            while j < len(s) and s[j] not in "() \t\n":
                j += 1
            tokens.append(s[i:j])
            i = j
    return tokens


def _parse(tokens: list, pos: list) -> Any:
    """Parse one expression; pos[0] is current index. Returns tree: (op, args) or atom string."""
    if pos[0] >= len(tokens):
        return None
    t = tokens[pos[0]]
    if t == "(":
        pos[0] += 1
        if pos[0] >= len(tokens):
            return None
        op = tokens[pos[0]]
        pos[0] += 1
        args = []
        while pos[0] < len(tokens) and tokens[pos[0]] != ")":
            args.append(_parse(tokens, pos))
        if pos[0] < len(tokens):
            pos[0] += 1  # consume )
        return (op, args)
    elif t == ")":
        return None
    else:
        pos[0] += 1
        return t  # atom


def _eval_tree(tree: Any, state: Dict[str, str]) -> bool:
    """
    Evaluate parsed SMT-LIB2 Boolean tree against state.
    Atoms: resource__value -> state[resource] == value; true/false literals.
    """
    if tree is None:
        return False
    if isinstance(tree, str):
        lower = tree.strip().lower()
        if lower == "true":
            return True
        if lower == "false":
            return False
        if "__" in tree:
            resource, value = tree.split("__", 1)
            return state.get(resource) == value
        return False
    op, args = tree
    if op == "and":
        return all(_eval_tree(a, state) for a in args)
    if op == "or":
        return any(_eval_tree(a, state) for a in args)
    if op == "not":
        return not _eval_tree(args[0], state) if args else False
    if op == "=>":
        return not _eval_tree(args[0], state) or _eval_tree(args[1], state) if len(args) >= 2 else False
    if op == "=":
        # iff: (= φ ψ) ↔ (φ ⇔ ψ)
        return _eval_tree(args[0], state) == _eval_tree(args[1], state) if len(args) >= 2 else False
    return False


def state_to_smt(state: Dict[str, str]) -> str:
    """Encode concrete state as SMT conjunction: (and r1__v1 r2__v2 ...)."""
    if not state:
        return "true"
    parts = [f"{r}__{v}" for r, v in sorted(state.items())]
    return "(and " + " ".join(parts) + ")"


def state_from_resources(resources: Any) -> Dict[str, str]:
    """
    Build state dict from skillset client resources.
    Returns resource names and values as reported by the client so that SMT formulas
    in the robust model (e.g. motor_state__On, spot_state__Sitting) evaluate correctly.
    """
    state = {}
    if hasattr(resources, "motor_state"):
        v = (getattr(resources, "motor_state", "") or "").strip()
        state["motor_state"] = v if v in ("On", "Off") else ("On" if v.lower() == "on" else "Off")
    if hasattr(resources, "spot_state"):
        v = (getattr(resources, "spot_state", "") or "").strip()
        state["spot_state"] = v if v in ("Sitting", "Standing") else ("Standing" if v.lower() == "standing" else "Sitting")
    return state


class StateChecker:
    """Check configuration membership in SMT-described regions."""

    def __init__(self, state: Dict[str, str]):
        self._state = dict(state)

    def satisfies(self, formula_smt: str) -> bool:
        """Return True iff current state satisfies the formula."""
        if not formula_smt or formula_smt.strip().lower() == "false":
            return False
        if formula_smt.strip().lower() == "true":
            return True
        tokens = _tokenize(formula_smt)
        pos = [0]
        tree = _parse(tokens, pos)
        return _eval_tree(tree, self._state)

    @property
    def state(self) -> Dict[str, str]:
        return dict(self._state)
