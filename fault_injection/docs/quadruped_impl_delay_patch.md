# Skill Execution Delay for quadruped_impl

This delay is **already applied** in `quadruped_impl/src/Node.cpp` and `include/QuadrupedImplNode.hpp`. The parameter `skill_execution_delay_sec` defaults to 0 (no delay); set it to a positive value to create a window for fault injection during skills.

## Original Patch for `Node.cpp` (for reference)

Add a sleep at the start of each `skill_*_on_start()` implementation, before the success call:

```cpp
// At the top of Node.cpp, after includes:
#include <chrono>
#include <thread>

// Helper (add to QuadrupedImplNode or as static):
static void maybe_delay(rclcpp::Node* node) {
    if (!node->has_parameter("skill_execution_delay_sec")) return;
    double sec = node->get_parameter("skill_execution_delay_sec").as_double();
    if (sec > 0)
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(sec * 1000)));
}
```

Then in each skill callback, insert `maybe_delay(this);` **before** the success call:

```cpp
void QuadrupedImplNode::skill_standup_on_start(){
    RCLCPP_INFO_STREAM(this->get_logger(), "Standing up...");
    maybe_delay(this);  // <-- ADD: gives fault injector time to trigger
    this->skill_standup_success_is_standing();
    RCLCPP_INFO_STREAM(this->get_logger(), "Robot standing");
}
```

Repeat for all skills: `skill_init_power_on_start`, `skill_safe_poweroff_on_start`, `skill_standup_on_start`, `skill_sitdown_on_start`, `skill_go_to_on_start`, `skill_recharge_on_start`, `skill_take_picture_on_start`.

## Parameter Declaration

In `QuadrupedImplNode` constructor (in Node.cpp or the impl node init):

```cpp
this->declare_parameter("skill_execution_delay_sec", 0.0);
```

## Usage

```bash
# Run with 1.5s delay per skill
ros2 run quadruped_impl quadruped_impl_node --ros-args -p skill_execution_delay_sec:=1.5
```

With this delay, the fault injection node can trigger `on_skill_request` (inject when a skill request is received) and the fault will take effect **during** the skill’s execution window.
