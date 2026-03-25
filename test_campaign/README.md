# test_campaign

Library for test campaign: orchestrates simulation batches, collects execution traces, stores run records.

## How to Run Batches

**You do not manually run any nodes.** The `run_campaign.py` script launches the full stack for each run, collects traces, and stores records.

## Executors

| Executor | Stack | Description |
|----------|-------|-------------|
| `manager` | skill_execution_manager  | Full robust execution path with skill_execution_manager. |
| `naive` | quadruped_mission  | Simple replan-on-failure: executes plan step-by-step, observes config on skill failure, updates PDDL problem, replans with FF, retries. No skill_execution_manager. Use for comparison with the robust path. |

### Naive executor

```bash
ros2 run test_campaign run_campaign.py --batch fault_injection --executor naive -v
```

The naive executor uses `quadruped_mission.py`, which launches the quadruped_impl, fault_injection_node, and mission. It runs FF for planning and replanning on failure.

## Batch Types

| Batch | Description |
|-------|-------------|
| `nominal` | 1 run: no fault injection, no skill failure |
| `faulty_pddl` | 11 runs: See [PDDL_models/README.md](../deliberative_layer/PDDL_models/README.md) for more info on the `faulty_pddl` batch. |
| `fault_injection` | 14 runs: each skill × each resource; event (change resource value) triggered right after the skill completes |
| `skill_failure` | 7 runs: each skill fails once (always-fail would block the plan) |

### Procedure

1. Build and source the workspace.

2. Run the campaign script with the selected batch type(s):
   ```bash
   ros2 run test_campaign run_campaign.py --batch [batch_name]
   ros2 run test_campaign run_campaign.py --batch nominal --batch fault_injection --executor naive -v
   ```

## Store

Records are kept in `~/.ros/test_campaign/runs.jsonl` (or `--store` path). 
Each line is a JSON object with `run_id`, `batch_type`, `config`, `alterations`, `plan_initial`, `skill_executions`, `replans`, `robust_skills_triggered`, `outcome`, `timestamp`, `duration_sec`.

### Options

| Option | Default | Description |
|--------|---------|-------------|
| `--batch` | (required) | Batch type: `nominal`, `faulty_pddl`, `fault_injection`, `skill_failure` |
| `--executor` | `manager` | `manager`: skill_execution_manager + deliberative; `naive`: quadruped_mission (replan on failure) |
| `--trace-topic` | `execution_trace` | Topic for trace messages |
| `--store` | `~/.ros/test_campaign/runs.jsonl` | Path to JSONL store |
| `--timeout` | `300.0` | Seconds to wait for plan_complete per run |
| `--reset-delay` | `3.0` | Seconds between runs for clean shutdown (DDS, nodes) |
| `--startup-timeout` | `30.0` | Seconds to wait for plan_start before aborting (detects startup hangs) |
| `--verbose`, `-v` | off | Show launch output and deliberative summary (plan_initial, replans) |


