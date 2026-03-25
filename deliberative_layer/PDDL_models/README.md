# PDDL Models

Numbered folders contain PDDL domain and problem pairs for fault injection. Each folder holds `quadruped_example_domain.pddl` and `quadruped_example_problem.pddl`. Folders 1–7 alter the domain (relaxed resource preconditions); folders 8–11 alter the problem (initial conditions).

All folders 1–7 keep the same predicates and actions as the nominal model. Only preconditions over `resource_motor_state` and `resource_spot_state` are removed to produce faulty but still solvable plans.

## Folder Alterations

### Domain alterations (folders 0–7)

| Folder | Description | Action(s) without resource preconditions |
|--------|-------------|------------------------------------------|
| 0 | Nominal (baseline) | — |
| 1 | Fault injection variant | `init_power` |
| 2 | Fault injection variant | `safe_poweroff` |
| 3 | Fault injection variant | `standup` |
| 4 | Fault injection variant | `sitdown` |
| 5 | Fault injection variant | `go_to` |
| 6 | Fault injection variant | `recharge` |
| 7 | Fault injection variant | All actions (entire model) |

### Initial condition alterations (folders 8–11)

| Folder | Description | Altered Initial |
|--------|-------------|-----------------|
| 8 | Fault injection variant | `resource_motor_state`: OFF → ON |
| 9 | Fault injection variant | `resource_spot_state`: SITTING → STANDING |
| 10 | Fault injection variant | `at_location`: L0 → L1 |
| 11 | Fault injection variant | `at_location`: L0 → L2 |

## Action Order (reference)

1. `init_power` — power on (sitting, motor off → motor on)
2. `safe_poweroff` — power off (sitting, motor on → motor off)
3. `standup` — stand (sitting → standing)
4. `sitdown` — sit (standing → sitting)
5. `go_to` — move between locations (no resource altered)
6. `recharge` — charge at charging station (no resource altered)
7. `take_picture` — take picture at location (no resource altered)

## Usage

Select a folder via the `pddl_model_folder` parameter (default: 0):

```bash
ros2 launch deliberative_layer deliberative_launch.py pddl_model_folder:=3
```
