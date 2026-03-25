# deliberative_layer

ROS2 node (deliberative layer) that plans with FF and sends plans to the skill execution manager. On execution failure, reads the current resource configuration from the failure response, updates the PDDL problem initial state, and re-plans with FF. The domain is never altered. Stops after more than 3 re-planning cycles.

## Dependencies

- FF planner (`ff` on PATH)
- `skill_execution_manager` (execution_manager_node)
- `quadruped_impl` (quadruped_impl_node)

## Usage

```bash
# Terminal 1: quadruped skillset (functional layer)
ros2 run quadruped_impl quadruped_impl_node

# Terminal 2: skill execution manager (executive layer)
ros2 run skill_execution_manager execution_manager_node.py

# Terminal 3: deliberative layer
ros2 run deliberative_layer deliberative_node.py
```

Or use the launch file:

```bash
source install/setup.bash
ros2 launch deliberative_layer deliberative_launch.py
```

With custom paths:

```bash
ros2 launch deliberative_layer deliberative_launch.py \
  domain_path:=/path/to/domain.pddl \
  problem_path:=/path/to/problem.pddl
```

With a specific PDDL model folder (for fault injection variants):

```bash
ros2 launch deliberative_layer deliberative_launch.py pddl_model_folder:=1
```

## PDDL Model Structure

The `PDDL_models/` directory contains numbered subfolders (0, 1, 2, ...), each with a PDDL domain and problem pair. Folder `0` is the default (nominal case). Higher numbers can be used for fault injection variants (e.g., modified domains or problems for testing robustness).

> See the dedicated [README](./PDDL_models/README.md)

## Parameters
- `domain_path`: PDDL domain (default: package share PDDL_models/{pddl_model_folder}/quadruped_example_domain.pddl)
- `problem_path`: PDDL problem (default: package share PDDL_models/{pddl_model_folder}/quadruped_example_problem.pddl)
- `pddl_model_folder`: Folder number (0, 1, 2, ...) for PDDL model selection; default 0 (nominal); higher numbers for fault injection variants
- `ff_binary`: FF planner binary (default: "ff")
- `execution_manager_service`: Service name (default: "execute_plan")
- `max_retries`: Max re-plan attempts on failure (default: 3)