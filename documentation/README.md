# Robust Skills Workspace

ROS2 workspace for robust skill execution and deliberative planning.

## Core Packages (Required)

Minimal stack for robust skill execution:

| Layer | Package | Role |
|-------|---------|------|
| **Deliberative** | [deliberative_layer](../deliberative_layer) | Plans with FF, sends plans to the execution manager, re-plans on failure (max 3 retries). |
| **Executive** | [skill_execution_manager](../skill_execution_manager) + [quadruped_skillset](../quadruped_skillset) | `skill_execution_manager` Executes plans using a **robust skill model**. Observes configuration, classifies into Nominal/Unrecoverable/Recoverable, runs enablement paths when possible, executes skills, escalates on failure. Exposes the `ExecutePlan` service. `quadruped_skillset` is mostly generated from a robot language (.rl) model. Orchestrates resource state (motor_state, spot_state, at_location) and enforces constraints for skill execution (standup, sitdown, go_to, etc.).|
| **Functional** | [quadruped_skillset](../quadruped_skillset)/`quadruped_impl` | Contains the hooks and callbacks supposed to communicate with the robot. In this simplified architecture, we emulate the functional layer directly within `quadruped_impl`.  |

## Quick Start

```bash
# Build and source (use ./build.sh to suppress pkg_resources deprecation warnings)
./build.sh
# or: colcon build
source install/setup.bash

# Run the full stack
ros2 launch deliberative_layer deliberative_launch.py
```

## Test Campaign

The **test_campaign** package orchestrates simulation batches for robustness evaluation. It launches the full stack per run, collects execution traces, and stores records in `~/.ros/test_campaign/runs.jsonl` (or `--store` path).

- **[fault_injection](../fault_injection) (optional)** — Optional node to inject faults (resource changes) during mission execution. Used by test_campaign for `fault_injection` and `skill_failure` batches.
- **[test_campaign](../test_campaign) (optional)** — Optional node to orchestrate test batches. See that folder’s README for more information.

| Batch | Description |
|-------|-------------|
| `nominal` | 1 run: pddl_folder=0, no fault injection, no skill failure |
| `faulty_pddl` | 11 runs: domain variants with one action removed each |
| `fault_injection` | 14 runs: inject resource changes after each skill |
| `skill_failure` | 7 runs: each skill fails once (always-fail would block the plan) |

## Test Campaign results 

Aggregated execution traces for batch runs are not bundled in this artifact; they can be reproduced locally via `test_campaign` (see `~/.ros/test_campaign/runs.jsonl` after runs).

## Optional Packages

- **gui_tools** — C++ library for ImGui-based applications. Used by GUI widgets (e.g. `quadruped_example_skillset_gui_widgets`) if you want a graphical interface. Skip it if you only need headless execution.
- **skills2pddl** — Standalone Python tool to generate PDDL domain/problem from skill definitions. Use it if you prefer generating PDDL from another format; otherwise you can write PDDL manually.

## External Packages

The **robust skill model** (JSON with SMT-LIB2 formulas for nominal/recoverable/unrecoverable regions) is produced by the external **skill_reinforcement** library (Rust, `sr_core`).

> The robust skill model library (`skill_reinforcement_library`, Rust) can regenerate models; an example exported JSON shipped with the executive layer is [quadruped_example.json](../skill_execution_manager/robust_model/quadruped_example.json).
