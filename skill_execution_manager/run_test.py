#!/usr/bin/env python3
"""
Test mode: run the skill execution manager with a .ff plan and .json robust model
using a mock executor (no ROS/skillset required).

Usage:
  python3 run_test.py
  python3 run_test.py --plan PDDL_plan/faulty_plan.ff --model robust_model/quadruped_example.json
  python3 run_test.py -p path/to/plan.ff -m path/to/robust.json

Run from this directory with PYTHONPATH=. or after sourcing the workspace install.
"""

import argparse
from pathlib import Path


def main() -> None:
    script_dir = Path(__file__).resolve().parent
    default_plan = script_dir / "PDDL_plan" / "plan.ff"
    default_model = script_dir / "robust_model" / "quadruped_example.json"

    parser = argparse.ArgumentParser(
        description="Run skill execution manager in test mode (mock executor)."
    )
    parser.add_argument(
        "-p", "--plan",
        type=Path,
        default=default_plan,
        help=f"Path to FF plan file (.ff). Default: {default_plan}",
    )
    parser.add_argument(
        "-m", "--model",
        type=Path,
        default=default_model,
        help=f"Path to robust model JSON. Default: {default_model}",
    )
    parser.add_argument(
        "-q", "--quiet",
        action="store_true",
        help="Minimal output (only completion status).",
    )
    args = parser.parse_args()

    from skill_execution_manager_core.test_mode import run_test_mode

    try:
        completed = run_test_mode(
            args.plan,
            args.model,
            verbose=not args.quiet,
        )
        exit(0 if completed else 1)
    except FileNotFoundError as e:
        print(e)
        exit(2)


if __name__ == "__main__":
    main()
