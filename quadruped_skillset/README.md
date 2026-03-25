# quadruped_skillset

Executive-layer package that orchestrates resource state (motor_state, spot_state, at_location) and enforces constraints for skill execution (standup, sitdown, go_to, recharge, take_picture, etc.). Mostly generated from a robot language (.rl) model.

Includes **quadruped_impl** for the functional layer (hooks and callbacks to communicate with the robot; emulated in this simplified architecture).

See the [documentation README](../documentation/README.md) for how it fits into the full stack.

---

## Naive executor (quadruped_mission)

`quadruped_mission.py` provides a simple replan-on-failure executor used by the test campaign with `--executor naive`. It does **not** use skill_execution_manager or deliberative_layer; instead it:

- Runs FF for planning and replanning
- Executes plan step-by-step via the skillset client
- On skill failure: observes current config, updates PDDL problem, replans, retries (max 3 replans)
- Publishes trace events for test_campaign (plan_start, skill_end, replan, plan_complete)

**Usage:** Typically launched via `test_campaign`:

```bash
ros2 run test_campaign run_campaign.py --batch fault_injection --executor naive -v
```

### quadruped_mission parameters

| Parameter | Default | Description |
|-----------|---------|-------------|
| `execution_trace_topic` | `execution_trace` | Topic for trace messages (plan_start, skill_end, replan, plan_complete) |
| `pddl_model_folder` | `0` | PDDL model folder index (from deliberative_layer share) |
| `post_skill_delay_sec` | `0.0` | Delay after each skill (used with fault_injection to give injector time) |

---

# Important note: 
The gui code has been slightly edited to account for instantaneous skill executions. If you re-generate the codes from quadruped_example.rl using robot-language, you may break the GUI.

Quick fix for `*_gui_widgets/src/main.cpp`: 

```py
while (gui.update() && rclcpp::ok())
{
gui_tools::processEvent(0.0f);
executor.spin_some(std::chrono::milliseconds(0));
}
```
