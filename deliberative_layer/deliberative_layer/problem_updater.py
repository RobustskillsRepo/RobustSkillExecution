"""
Update PDDL problem initial state with current resource configuration.
Does not alter the domain.

IMPORTANT: The executor may update the initial state for replanning purposes,
but the (:goal ...) section is NEVER modified. The goal must remain invariant.
"""

import re
from pathlib import Path
from typing import Any, Dict


# Map from skillset config (motor_state, spot_state) to PDDL constant names
MOTOR_STATE_MAP = {
    "off": "RESOURCE_MOTOR_STATE_STATE_OFF",
    "on": "RESOURCE_MOTOR_STATE_STATE_ON",
}
SPOT_STATE_MAP = {
    "sitting": "RESOURCE_SPOT_STATE_STATE_SITTING",
    "standing": "RESOURCE_SPOT_STATE_STATE_STANDING",
}

# Valid PDDL location objects (must match problem's :objects)
VALID_LOCATIONS = ("L0", "L1", "L2")


def update_problem_init_with_config(
    problem_path: Path,
    config: Dict[str, Any],
    output_path: Path,
) -> bool:
    """
    Update the problem's (:init ...) with the current resource configuration.
    config: dict with keys:
      - motor_state, spot_state (values: On/Off, Sitting/Standing)
      - at_location (e.g. L0, L1, L2)
      - picture_taken_at: list of locations where picture was taken (e.g. ["L2"])
      - charged: bool, True if recharge has succeeded
    Writes the modified problem to output_path.
    Returns True on success.

    The (:goal ...) section is NEVER altered. Only (:init ...) is updated.
    """
    original_content = problem_path.read_text()
    content = original_content
    motor = (config.get("motor_state") or "").strip().lower()
    spot = (config.get("spot_state") or "").strip().lower()
    at_loc = (config.get("at_location") or "").strip()
    motor_const = MOTOR_STATE_MAP.get(motor, MOTOR_STATE_MAP.get("off"))
    spot_const = SPOT_STATE_MAP.get(spot, SPOT_STATE_MAP.get("sitting"))

    # Extract and modify only the (:init ...) section; leave (:goal ...) unchanged
    init_pattern = r"\(:init\s+(.*?)\n\s*\)\s*\n"
    match = re.search(init_pattern, content, re.DOTALL)
    if not match:
        return False

    init_block = match.group(1)
    init_block = re.sub(
        r"\(resource_motor_state\s+RESOURCE_MOTOR_STATE_STATE_\w+\)",
        f"(resource_motor_state {motor_const})",
        init_block,
    )
    init_block = re.sub(
        r"\(resource_spot_state\s+RESOURCE_SPOT_STATE_STATE_\w+\)",
        f"(resource_spot_state {spot_const})",
        init_block,
    )
    if at_loc and at_loc.upper() in VALID_LOCATIONS:
        init_block = re.sub(
            r"\(at_location\s+\w+\)",
            f"(at_location {at_loc.upper()})",
            init_block,
        )

    # Add/update achieved predicates: picture_taken_at, charged
    init_block = init_block.rstrip()
    if not init_block.endswith("\n"):
        init_block += "\n"

    picture_locs = config.get("picture_taken_at")
    if isinstance(picture_locs, (list, tuple)):
        init_block = re.sub(r"\s*\(picture_taken_at\s+\w+\)\s*\n?", "", init_block)
        for loc in picture_locs:
            loc_upper = str(loc).strip().upper()
            if loc_upper in VALID_LOCATIONS:
                init_block += f"        (picture_taken_at {loc_upper})\n"
    elif picture_locs is not None and not isinstance(picture_locs, (list, tuple)):
        loc_upper = str(picture_locs).strip().upper()
        if loc_upper in VALID_LOCATIONS:
            init_block = re.sub(r"\s*\(picture_taken_at\s+\w+\)\s*\n?", "", init_block)
            init_block += f"        (picture_taken_at {loc_upper})\n"

    charged_val = config.get("charged")
    if charged_val is True:
        if "(charged)" not in init_block:
            init_block += "        (charged)\n"
    elif charged_val is False:
        init_block = re.sub(r"\s*\(charged\)\s*\n?", "", init_block)

    new_init = f"(:init\n{init_block}\n    )\n"
    content = re.sub(r"\(:init\s+.*?\n\s*\)\s*\n", new_init, content, flags=re.DOTALL)

    # Verify goal was not altered (safeguard: executor may update init, never goal)
    goal_marker = "(:goal"
    if goal_marker in content and goal_marker in original_content:
        if content[content.find(goal_marker) :] != original_content[original_content.find(goal_marker) :]:
            raise RuntimeError("update_problem_init_with_config must not alter the (:goal ...) section")
    output_path.write_text(content)
    return True
