#pragma once

#include "client.hpp"


class QuadrupedExampleSkillsetWidget: public QuadrupedExampleSkillsetClient
{
  public:
    QuadrupedExampleSkillsetWidget(const std::string &name, rclcpp::Node::SharedPtr node,
      bool display_data = true, bool display_resources = true, bool display_events = true);

    void update();
    bool update_window();
    void process();
    
    void display_start_init_power();
    
    
    void display_start_safe_poweroff();
    
    
    void display_start_standup();
    
    
    void display_start_sitdown();
    
    
    void display_start_go_to();
    inline void set_go_to_inputs(quadruped_example_skillset_interfaces::msg::SkillGoToInput input) { go_to_input_ = input; };
    inline void set_go_to_input_target(std_msgs::msg::String input) { go_to_input_.target = input; };
    
    
    
    void display_start_recharge();
    
    
    void display_start_take_picture();
    
    
  private:
    bool display_data_;
    bool display_resources_;
    bool display_events_;

    
    void event_row_set_standing();
    void event_button_set_standing();
    
    void event_row_set_sitting();
    void event_button_set_sitting();
    
    void event_row_set_poweroff();
    void event_button_set_poweroff();
    
    void event_row_set_poweron();
    void event_button_set_poweron();
    
    double event_response_timeout_;

    void skill_response_text(int result_code); 
    void skill_state_button(std::string skill, unsigned int state);
    
    bool subscribe_location_;
    
    bool subscribe_battery_;
    
    
    bool active_init_power_;
    
    bool active_safe_poweroff_;
    
    bool active_standup_;
    
    bool active_sitdown_;
    
    bool active_go_to_;
    
    bool active_recharge_;
    
    bool active_take_picture_;
    
};
