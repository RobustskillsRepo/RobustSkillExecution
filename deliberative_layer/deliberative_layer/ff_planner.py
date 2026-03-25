"""
Run FF planner on domain and problem, return path to plan file.
"""

import subprocess
import tempfile
from pathlib import Path


def run_ff(
    domain_path: Path,
    problem_path: Path,
    ff_binary: str = "ff",
    output_dir: Path | None = None,
) -> Path | None:
    """
    Run FF planner. Returns path to the generated plan.ff file, or None if no plan found.
    FF prints the plan to stdout; we capture it and write to plan.ff.
    """
    domain_path = Path(domain_path)
    problem_path = Path(problem_path)
    if not domain_path.exists() or not problem_path.exists():
        return None

    out_dir = Path(output_dir) if output_dir else Path(tempfile.mkdtemp())
    plan_path = out_dir / "plan.ff"

    try:
        result = subprocess.run(
            [ff_binary, "-o", str(domain_path), "-f", str(problem_path)],
            capture_output=True,
            text=True,
            timeout=30,
        )
    except FileNotFoundError:
        return None
    except subprocess.TimeoutExpired:
        return None

    stdout = result.stdout or ""
    if "found legal plan" not in stdout.lower():
        return None

    plan_path.write_text(stdout)
    return plan_path
