#pragma once


#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/empty.hpp>
#include <quadruped_example_skillset_interfaces/msg/skillset_status.hpp>
#include <quadruped_example_skillset_interfaces/msg/event_request.hpp>
#include <quadruped_example_skillset_interfaces/msg/event_response.hpp>
#include <quadruped_example_skillset_interfaces/msg/data_request.hpp>
#include <quadruped_example_skillset_interfaces/msg/skill_interrupt.hpp>

#include <quadruped_example_skillset_interfaces/msg/data_location.hpp>
#include <quadruped_example_skillset_interfaces/msg/data_location_response.hpp>

#include <quadruped_example_skillset_interfaces/msg/data_battery.hpp>
#include <quadruped_example_skillset_interfaces/msg/data_battery_response.hpp>


#include <quadruped_example_skillset_interfaces/msg/skill_init_power_status.hpp>



#include <quadruped_example_skillset_interfaces/msg/skill_init_power_request.hpp>
#include <quadruped_example_skillset_interfaces/msg/skill_init_power_response.hpp>

#include <quadruped_example_skillset_interfaces/msg/skill_safe_poweroff_status.hpp>



#include <quadruped_example_skillset_interfaces/msg/skill_safe_poweroff_request.hpp>
#include <quadruped_example_skillset_interfaces/msg/skill_safe_poweroff_response.hpp>

#include <quadruped_example_skillset_interfaces/msg/skill_standup_status.hpp>



#include <quadruped_example_skillset_interfaces/msg/skill_standup_request.hpp>
#include <quadruped_example_skillset_interfaces/msg/skill_standup_response.hpp>

#include <quadruped_example_skillset_interfaces/msg/skill_sitdown_status.hpp>



#include <quadruped_example_skillset_interfaces/msg/skill_sitdown_request.hpp>
#include <quadruped_example_skillset_interfaces/msg/skill_sitdown_response.hpp>

#include <quadruped_example_skillset_interfaces/msg/skill_go_to_status.hpp>
#include <quadruped_example_skillset_interfaces/msg/skill_go_to_input.hpp>


#include <quadruped_example_skillset_interfaces/msg/skill_go_to_request.hpp>
#include <quadruped_example_skillset_interfaces/msg/skill_go_to_response.hpp>

#include <quadruped_example_skillset_interfaces/msg/skill_recharge_status.hpp>



#include <quadruped_example_skillset_interfaces/msg/skill_recharge_request.hpp>
#include <quadruped_example_skillset_interfaces/msg/skill_recharge_response.hpp>

#include <quadruped_example_skillset_interfaces/msg/skill_take_picture_status.hpp>



#include <quadruped_example_skillset_interfaces/msg/skill_take_picture_request.hpp>
#include <quadruped_example_skillset_interfaces/msg/skill_take_picture_response.hpp>


class QuadrupedExampleSkillsetClient
{
  public:
    QuadrupedExampleSkillsetClient(const std::string &name, rclcpp::Node::SharedPtr node);
    inline quadruped_example_skillset_interfaces::msg::SkillsetStatus get_status() { return status_; };

    //----- skills
    
    std::string start_init_power();
    void interrupt_init_power(std::string id);
    void interrupt_init_power();
    
    std::string start_safe_poweroff();
    void interrupt_safe_poweroff(std::string id);
    void interrupt_safe_poweroff();
    
    std::string start_standup();
    void interrupt_standup(std::string id);
    void interrupt_standup();
    
    std::string start_sitdown();
    void interrupt_sitdown(std::string id);
    void interrupt_sitdown();
    
    std::string start_go_to();
    void interrupt_go_to(std::string id);
    void interrupt_go_to();
    
    std::string start_recharge();
    void interrupt_recharge(std::string id);
    void interrupt_recharge();
    
    std::string start_take_picture();
    void interrupt_take_picture(std::string id);
    void interrupt_take_picture();
    
    //----- data getters
    
    inline quadruped_example_skillset_interfaces::msg::DataLocationResponse get_data_location() const { return data_location_; };
    void create_data_location_subscription();
    
    inline quadruped_example_skillset_interfaces::msg::DataBatteryResponse get_data_battery() const { return data_battery_; };
    void create_data_battery_subscription();
    
    //----- resource getters
    
    inline std::string get_resource_motor_state() const { return resource_state_.at("motor_state"); };
    
    inline std::string get_resource_spot_state() const { return resource_state_.at("spot_state"); };
    

  protected:
    std::string name_;
    rclcpp::Node::SharedPtr node_;

    //----- status
    quadruped_example_skillset_interfaces::msg::SkillsetStatus status_;
    void request_status();
    double time_since_status();
    //----- events
    std::map<std::string, quadruped_example_skillset_interfaces::msg::EventResponse> events_;
    std::map<std::string, std::string> events_ids_;
    std::map<std::string, rclcpp::Time> events_stamps_;
    std::string send_event(std::string event);
    double time_since_event(std::string event) const;
    //----- data
    
    quadruped_example_skillset_interfaces::msg::DataLocationResponse data_location_;
    std::string data_location_request();
    void destroy_data_location_subscription();
    
    quadruped_example_skillset_interfaces::msg::DataBatteryResponse data_battery_;
    std::string data_battery_request();
    void destroy_data_battery_subscription();
    
    //----- resources
    std::map<std::string, std::string> resource_state_;
    //----- skills
    
    quadruped_example_skillset_interfaces::msg::SkillInitPowerStatus init_power_status_;
    
    
    
    quadruped_example_skillset_interfaces::msg::SkillInitPowerResponse init_power_result_;
    
    quadruped_example_skillset_interfaces::msg::SkillSafePoweroffStatus safe_poweroff_status_;
    
    
    
    quadruped_example_skillset_interfaces::msg::SkillSafePoweroffResponse safe_poweroff_result_;
    
    quadruped_example_skillset_interfaces::msg::SkillStandupStatus standup_status_;
    
    
    
    quadruped_example_skillset_interfaces::msg::SkillStandupResponse standup_result_;
    
    quadruped_example_skillset_interfaces::msg::SkillSitdownStatus sitdown_status_;
    
    
    
    quadruped_example_skillset_interfaces::msg::SkillSitdownResponse sitdown_result_;
    
    quadruped_example_skillset_interfaces::msg::SkillGoToStatus go_to_status_;
    quadruped_example_skillset_interfaces::msg::SkillGoToInput go_to_input_;
    
    
    quadruped_example_skillset_interfaces::msg::SkillGoToResponse go_to_result_;
    
    quadruped_example_skillset_interfaces::msg::SkillRechargeStatus recharge_status_;
    
    
    
    quadruped_example_skillset_interfaces::msg::SkillRechargeResponse recharge_result_;
    
    quadruped_example_skillset_interfaces::msg::SkillTakePictureStatus take_picture_status_;
    
    
    
    quadruped_example_skillset_interfaces::msg::SkillTakePictureResponse take_picture_result_;
    

  private:
    rclcpp::QoS qos_best_;
    rclcpp::QoS qos_reliable_;

    std::string generate_id() const;

    //----- status
    rclcpp::Publisher<std_msgs::msg::Empty>::SharedPtr status_pub_;
    rclcpp::Subscription<quadruped_example_skillset_interfaces::msg::SkillsetStatus>::SharedPtr status_sub_;
    void status_callback_(const quadruped_example_skillset_interfaces::msg::SkillsetStatus::SharedPtr msg);
    //----- events
    rclcpp::Publisher<quadruped_example_skillset_interfaces::msg::EventRequest>::SharedPtr event_pub_;
    rclcpp::Subscription<quadruped_example_skillset_interfaces::msg::EventResponse>::SharedPtr event_sub_;
    void event_callback_(const quadruped_example_skillset_interfaces::msg::EventResponse::SharedPtr msg);
    //----- data
    
    rclcpp::Subscription<quadruped_example_skillset_interfaces::msg::DataLocation>::SharedPtr data_location_sub_;
    void data_location_callback_(const quadruped_example_skillset_interfaces::msg::DataLocation::SharedPtr msg);
    rclcpp::Publisher<quadruped_example_skillset_interfaces::msg::DataRequest>::SharedPtr data_location_pub_;
    rclcpp::Subscription<quadruped_example_skillset_interfaces::msg::DataLocationResponse>::SharedPtr data_location_response_;
    void data_location_response_callback_(const quadruped_example_skillset_interfaces::msg::DataLocationResponse::SharedPtr msg);
    
    rclcpp::Subscription<quadruped_example_skillset_interfaces::msg::DataBattery>::SharedPtr data_battery_sub_;
    void data_battery_callback_(const quadruped_example_skillset_interfaces::msg::DataBattery::SharedPtr msg);
    rclcpp::Publisher<quadruped_example_skillset_interfaces::msg::DataRequest>::SharedPtr data_battery_pub_;
    rclcpp::Subscription<quadruped_example_skillset_interfaces::msg::DataBatteryResponse>::SharedPtr data_battery_response_;
    void data_battery_response_callback_(const quadruped_example_skillset_interfaces::msg::DataBatteryResponse::SharedPtr msg);
    
    //----- skills
    
    rclcpp::Publisher<quadruped_example_skillset_interfaces::msg::SkillInitPowerRequest>::SharedPtr init_power_request_pub_;
    rclcpp::Publisher<quadruped_example_skillset_interfaces::msg::SkillInterrupt>::SharedPtr init_power_interrupt_pub_;
    rclcpp::Subscription<quadruped_example_skillset_interfaces::msg::SkillInitPowerResponse>::SharedPtr init_power_response_sub_;
    void init_power_response_callback(const quadruped_example_skillset_interfaces::msg::SkillInitPowerResponse::SharedPtr msg);
    
    
    rclcpp::Publisher<quadruped_example_skillset_interfaces::msg::SkillSafePoweroffRequest>::SharedPtr safe_poweroff_request_pub_;
    rclcpp::Publisher<quadruped_example_skillset_interfaces::msg::SkillInterrupt>::SharedPtr safe_poweroff_interrupt_pub_;
    rclcpp::Subscription<quadruped_example_skillset_interfaces::msg::SkillSafePoweroffResponse>::SharedPtr safe_poweroff_response_sub_;
    void safe_poweroff_response_callback(const quadruped_example_skillset_interfaces::msg::SkillSafePoweroffResponse::SharedPtr msg);
    
    
    rclcpp::Publisher<quadruped_example_skillset_interfaces::msg::SkillStandupRequest>::SharedPtr standup_request_pub_;
    rclcpp::Publisher<quadruped_example_skillset_interfaces::msg::SkillInterrupt>::SharedPtr standup_interrupt_pub_;
    rclcpp::Subscription<quadruped_example_skillset_interfaces::msg::SkillStandupResponse>::SharedPtr standup_response_sub_;
    void standup_response_callback(const quadruped_example_skillset_interfaces::msg::SkillStandupResponse::SharedPtr msg);
    
    
    rclcpp::Publisher<quadruped_example_skillset_interfaces::msg::SkillSitdownRequest>::SharedPtr sitdown_request_pub_;
    rclcpp::Publisher<quadruped_example_skillset_interfaces::msg::SkillInterrupt>::SharedPtr sitdown_interrupt_pub_;
    rclcpp::Subscription<quadruped_example_skillset_interfaces::msg::SkillSitdownResponse>::SharedPtr sitdown_response_sub_;
    void sitdown_response_callback(const quadruped_example_skillset_interfaces::msg::SkillSitdownResponse::SharedPtr msg);
    
    
    rclcpp::Publisher<quadruped_example_skillset_interfaces::msg::SkillGoToRequest>::SharedPtr go_to_request_pub_;
    rclcpp::Publisher<quadruped_example_skillset_interfaces::msg::SkillInterrupt>::SharedPtr go_to_interrupt_pub_;
    rclcpp::Subscription<quadruped_example_skillset_interfaces::msg::SkillGoToResponse>::SharedPtr go_to_response_sub_;
    void go_to_response_callback(const quadruped_example_skillset_interfaces::msg::SkillGoToResponse::SharedPtr msg);
    
    
    rclcpp::Publisher<quadruped_example_skillset_interfaces::msg::SkillRechargeRequest>::SharedPtr recharge_request_pub_;
    rclcpp::Publisher<quadruped_example_skillset_interfaces::msg::SkillInterrupt>::SharedPtr recharge_interrupt_pub_;
    rclcpp::Subscription<quadruped_example_skillset_interfaces::msg::SkillRechargeResponse>::SharedPtr recharge_response_sub_;
    void recharge_response_callback(const quadruped_example_skillset_interfaces::msg::SkillRechargeResponse::SharedPtr msg);
    
    
    rclcpp::Publisher<quadruped_example_skillset_interfaces::msg::SkillTakePictureRequest>::SharedPtr take_picture_request_pub_;
    rclcpp::Publisher<quadruped_example_skillset_interfaces::msg::SkillInterrupt>::SharedPtr take_picture_interrupt_pub_;
    rclcpp::Subscription<quadruped_example_skillset_interfaces::msg::SkillTakePictureResponse>::SharedPtr take_picture_response_sub_;
    void take_picture_response_callback(const quadruped_example_skillset_interfaces::msg::SkillTakePictureResponse::SharedPtr msg);
    
    
};
