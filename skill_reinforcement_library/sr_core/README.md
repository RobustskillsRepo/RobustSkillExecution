# sr_core – core robustification library

`sr_core` provides:

- Basic modeling primitives: `Model`, `Resource`, `Skill`, and system properties.
- A **backward solver** (using Z3) to compute recovery paths for target skills.
- An **exporter** to build robust skill JSON models consumed by the execution manager.
- Built-in models under `sr_core/src/models/` (currently `quadruped`).

Z3 is required at runtime:

```console
sudo apt install z3
```

## Running from the workspace root

### Main CLI (recommended)

Use the main binary to select a model and either robustify a single skill (print only) or the whole skillset (export JSON):

```console
cargo run -p sr_core -- --help
```

Examples:

- Single skill recap (quadruped `go_to`):

  ```console
  cargo run -p sr_core -- \
    --model quadruped \
    --skill go_to \
    --max-cost 3 \
    --enum-cap 5 \
    -vv
  ```

- Whole skillset → JSON:

  ```console
  cargo run -p sr_core -- \
    --model quadruped \
    --max-cost 3 \
    --enum-cap 5 \
    --output-model-name quadruped_example
  ```

  This writes `robust_models/quadruped_example.json`.

### Example binary

You can also use the example directly:

```console
cargo run --example quadruped_example -- \
  --max-cost 3 \
  --enum-cap 5 \
  --model-name quadruped_example
```

## Logging

- Logs (SMT files, `nodes_summary.csv`, `edges_summary.csv`) are written under a unique per-run folder inside `log/` (e.g. `log/run-<timestamp>-<pid>/`).
- Logging is enabled by default; pass `--no-log` to disable SMT tracing.

