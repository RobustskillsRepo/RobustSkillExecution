# skill_execution_manager

Execution manager using robust skills. Observe configuration, classify into Nominam/Unrecoverable/Recoverable, run enablement path if needed, execute planned skill, then advance or escalate. 

Uses a **robust model** (JSON) produced by the external **skill_reinforcement** library (Rust, `sr_core`).

## Dependencies

- `rclpy`
- `quadruped_example_skillset_interfaces`
- `quadruped_example_skillset_client` (at runtime, for skill execution)

## ROS2 node

The execution manager runs as a ROS2 node that connects to `quadruped_impl_node` to execute skills.

```bash
# Terminal 1: Start the skillset (quadruped_impl)
ros2 run quadruped_impl quadruped_impl_node

# Terminal 2: Start the execution manager
ros2 run skill_execution_manager execution_manager_node.py
```

Or call the **ROS2 service** :

```bash
ros2 service call /execute_plan skill_execution_manager/srv/ExecutePlan \
  "{plan_ff_path: '/path/to/plan.ff', robust_model_path: ''}"
```
