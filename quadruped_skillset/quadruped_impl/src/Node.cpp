#include "QuadrupedImplNode.hpp"

#include <chrono>
#include <thread>

static void maybe_skill_delay(rclcpp::Node* node) {
    if (!node->has_parameter("skill_execution_delay_sec")) return;
    double sec = node->get_parameter("skill_execution_delay_sec").as_double();
    if (sec > 0)
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(sec * 1000)));
}

bool QuadrupedImplNode::should_fail_skill(const std::string& skill_name) {
    std::string mode = this->get_parameter("skill_failure_mode").as_string();
    std::string target = this->get_parameter("skill_failure_target").as_string();
    if (mode == "none" || target.empty()) return false;
    if (skill_name != target) return false;
    if (mode == "always") return true;
    if (mode == "once") {
        if (skills_already_failed_.count(skill_name)) return false;
        skills_already_failed_.insert(skill_name);
        return true;
    }
    return false;
}

void QuadrupedImplNode::skill_init_power_on_start(){
    RCLCPP_INFO_STREAM(this->get_logger(), 
          "Initializing power...");
    maybe_skill_delay(this);
    if (should_fail_skill("init_power")) {
        this->skill_init_power_failure_couldnot_on();
        return;
    }
    this->skill_init_power_success_is_poweredon();
    RCLCPP_INFO_STREAM(this->get_logger(), 
          "Power initialized");
}
   
void QuadrupedImplNode::skill_safe_poweroff_on_start(){
    RCLCPP_INFO_STREAM(this->get_logger(), 
          "Safely powering off...");
    maybe_skill_delay(this);
    if (should_fail_skill("safe_poweroff")) {
        this->skill_safe_poweroff_failure_couldnot_off();
        return;
    }
    this->skill_safe_poweroff_success_is_poweredoff();
    RCLCPP_INFO_STREAM(this->get_logger(), 
          "Powered off");
}
   
void QuadrupedImplNode::skill_standup_on_start(){
    RCLCPP_INFO_STREAM(this->get_logger(), 
          "Standing up...");
    maybe_skill_delay(this);
    if (should_fail_skill("standup")) {
        this->skill_standup_failure_couldnot_stand();
        return;
    }
    this->skill_standup_success_is_standing();
    RCLCPP_INFO_STREAM(this->get_logger(), 
          "Robot standing");
}
    
void QuadrupedImplNode::skill_sitdown_on_start(){
    RCLCPP_INFO_STREAM(this->get_logger(), 
          "Sitting down...");
    maybe_skill_delay(this);
    if (should_fail_skill("sitdown")) {
        this->skill_sitdown_failure_couldnot_sit();
        return;
    }
    this->skill_sitdown_success_is_sitting();
    RCLCPP_INFO_STREAM(this->get_logger(), 
          "Robot sitting");
}
    
void QuadrupedImplNode::skill_go_to_on_start(){
    RCLCPP_INFO_STREAM(this->get_logger(), 
          "Going to target...");
    maybe_skill_delay(this);
    auto input = *this->skill_go_to_input();
    current_location.target = input.target;
    this->set_data_location(current_location.target);
    if (should_fail_skill("go_to")) {
        this->skill_go_to_failure_not_arrived();
        return;
    }
    this->skill_go_to_success_is_arrived();
    RCLCPP_INFO_STREAM(this->get_logger(), 
          "Arrived at target");
}
 
bool QuadrupedImplNode::skill_recharge_validate_hook(){
    if (current_location.target.data != "L1"){ 
        RCLCPP_ERROR(this->get_logger(), 
            "Cannot recharge because not at charging station");
        return false;
    }
    return true;
}   

void QuadrupedImplNode::skill_recharge_on_start(){
    RCLCPP_INFO_STREAM(this->get_logger(), 
          "Recharging...");
    maybe_skill_delay(this);
    if (should_fail_skill("recharge")) {
        this->skill_recharge_failure_couldnot_charge();
        return;
    }
    this->skill_recharge_success_has_charged();
    RCLCPP_INFO_STREAM(this->get_logger(), 
          "Recharged");
}

void QuadrupedImplNode::skill_take_picture_on_start(){
    RCLCPP_INFO_STREAM(this->get_logger(), 
          "Taking picture...");
    maybe_skill_delay(this);
    if (should_fail_skill("take_picture")) {
        this->skill_take_picture_failure_pic_failed();
        return;
    }
    this->skill_take_picture_success_pic_taken();
    RCLCPP_INFO_STREAM(this->get_logger(), 
          "Picture taken");
}