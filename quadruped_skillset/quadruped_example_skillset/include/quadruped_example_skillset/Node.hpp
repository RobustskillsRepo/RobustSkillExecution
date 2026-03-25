#ifndef QUADRUPED_EXAMPLE_NODE_HPP
#define QUADRUPED_EXAMPLE_NODE_HPP

#include "Resource.hpp"

#include <string>
#include <memory>
#include <mutex>
#include <stdint.h>
#include <tuple>

#include "rclcpp/rclcpp.hpp"
#include "rclcpp/time.hpp"
#include "rclcpp/duration.hpp"
#include "std_msgs/msg/empty.hpp"
#include "std_msgs/msg/string.hpp"

#include "quadruped_example_skillset_interfaces/msg/skillset_status.hpp"
#include "quadruped_example_skillset_interfaces/msg/resource_state.hpp"
#include "quadruped_example_skillset_interfaces/msg/data_request.hpp"
#include "quadruped_example_skillset_interfaces/msg/event_request.hpp"
#include "quadruped_example_skillset_interfaces/msg/event_response.hpp"
#include "quadruped_example_skillset_interfaces/msg/skill_interrupt.hpp"
#include "quadruped_example_skillset_interfaces/msg/data_location.hpp"
#include "quadruped_example_skillset_interfaces/msg/data_location_response.hpp"
#include "quadruped_example_skillset_interfaces/msg/data_battery.hpp"
#include "quadruped_example_skillset_interfaces/msg/data_battery_response.hpp"


#include "quadruped_example_skillset_interfaces/msg/skill_init_power_request.hpp"
#include "quadruped_example_skillset_interfaces/msg/skill_init_power_response.hpp"


#include "quadruped_example_skillset_interfaces/msg/skill_safe_poweroff_request.hpp"
#include "quadruped_example_skillset_interfaces/msg/skill_safe_poweroff_response.hpp"


#include "quadruped_example_skillset_interfaces/msg/skill_standup_request.hpp"
#include "quadruped_example_skillset_interfaces/msg/skill_standup_response.hpp"


#include "quadruped_example_skillset_interfaces/msg/skill_sitdown_request.hpp"
#include "quadruped_example_skillset_interfaces/msg/skill_sitdown_response.hpp"


#include "quadruped_example_skillset_interfaces/msg/skill_go_to_input.hpp"
#include "quadruped_example_skillset_interfaces/msg/skill_go_to_request.hpp"
#include "quadruped_example_skillset_interfaces/msg/skill_go_to_response.hpp"


#include "quadruped_example_skillset_interfaces/msg/skill_recharge_request.hpp"
#include "quadruped_example_skillset_interfaces/msg/skill_recharge_response.hpp"


#include "quadruped_example_skillset_interfaces/msg/skill_take_picture_request.hpp"
#include "quadruped_example_skillset_interfaces/msg/skill_take_picture_response.hpp"



using namespace std::chrono_literals;

namespace quadruped_example_skillset
{

    enum class SkillState
    {
        Ready,
        Running,
        Interrupting
    };
    
    //-------------------- quadruped_example --------------------

    class QuadrupedExampleNodeDebug;

    class QuadrupedExampleNode : public rclcpp::Node
    {
        friend class QuadrupedExampleNodeDebug;

    public:
        QuadrupedExampleNode(const std::string node_name, const std::string info);
        virtual ~QuadrupedExampleNode() {}

        std::string info() const { return info_; }

    protected:
        //-------------------- Data --------------------
        //----- location -----
        quadruped_example_skillset_interfaces::msg::DataLocation get_data_location();
        void set_data_location(std_msgs::msg::String data);
        quadruped_example_skillset_interfaces::msg::DataLocation get_data_location_hook();
        void set_data_location_hook(std_msgs::msg::String data);
        //----- battery -----
        quadruped_example_skillset_interfaces::msg::DataBattery get_data_battery();
        void set_data_battery(std_msgs::msg::Float64 data);
        quadruped_example_skillset_interfaces::msg::DataBattery get_data_battery_hook();
        void set_data_battery_hook(std_msgs::msg::Float64 data);
        
        //-------------------- Resource --------------------
        std::string get_motor_state_state();
        std::string get_motor_state_state_hook();
        std::string get_spot_state_state();
        std::string get_spot_state_state_hook();
        
        //-------------------- Status --------------------
        quadruped_example_skillset_interfaces::msg::SkillsetStatus get_skillset_status();
        //-------------------- Event Hook --------------------
        virtual void event_set_standing_hook();
        void event_set_standing();
        virtual void event_set_sitting_hook();
        void event_set_sitting();
        virtual void event_set_poweroff_hook();
        void event_set_poweroff();
        virtual void event_set_poweron_hook();
        void event_set_poweron();
        
        //-------------------- Skill init_power --------------------
        inline SkillState skill_init_power_state() const { return skill_init_power_state_; }
        virtual bool skill_init_power_validate_hook();
        virtual void skill_init_power_start_hook();
        virtual void skill_init_power_on_start();
        virtual void skill_init_power_interrupt_hook();
        bool skill_init_power_success_is_poweredon();
        bool skill_init_power_failure_couldnot_on();
        
        //-------------------- Skill safe_poweroff --------------------
        inline SkillState skill_safe_poweroff_state() const { return skill_safe_poweroff_state_; }
        virtual bool skill_safe_poweroff_validate_hook();
        virtual void skill_safe_poweroff_start_hook();
        virtual void skill_safe_poweroff_on_start();
        virtual void skill_safe_poweroff_interrupt_hook();
        bool skill_safe_poweroff_success_is_poweredoff();
        bool skill_safe_poweroff_failure_couldnot_off();
        
        //-------------------- Skill standup --------------------
        inline SkillState skill_standup_state() const { return skill_standup_state_; }
        virtual bool skill_standup_validate_hook();
        virtual void skill_standup_start_hook();
        virtual void skill_standup_on_start();
        virtual void skill_standup_invariant_is_powered_hook();
        virtual void skill_standup_interrupt_hook();
        bool skill_standup_success_is_standing();
        bool skill_standup_failure_couldnot_stand();
        
        //-------------------- Skill sitdown --------------------
        inline SkillState skill_sitdown_state() const { return skill_sitdown_state_; }
        virtual bool skill_sitdown_validate_hook();
        virtual void skill_sitdown_start_hook();
        virtual void skill_sitdown_on_start();
        virtual void skill_sitdown_invariant_is_powered_hook();
        virtual void skill_sitdown_interrupt_hook();
        bool skill_sitdown_success_is_sitting();
        bool skill_sitdown_failure_couldnot_sit();
        
        //-------------------- Skill go_to --------------------
        const quadruped_example_skillset_interfaces::msg::SkillGoToInput::SharedPtr skill_go_to_input() const; 
        inline SkillState skill_go_to_state() const { return skill_go_to_state_; }
        virtual bool skill_go_to_validate_hook();
        virtual void skill_go_to_start_hook();
        virtual void skill_go_to_on_start();
        virtual void skill_go_to_invariant_is_standing_hook();
        virtual void skill_go_to_invariant_is_powered_hook();
        virtual void skill_go_to_interrupt_hook();
        bool skill_go_to_success_is_arrived();
        bool skill_go_to_failure_not_arrived();
        
        //-------------------- Skill recharge --------------------
        inline SkillState skill_recharge_state() const { return skill_recharge_state_; }
        virtual bool skill_recharge_validate_hook();
        virtual void skill_recharge_start_hook();
        virtual void skill_recharge_on_start();
        virtual void skill_recharge_invariant_is_sitting_hook();
        virtual void skill_recharge_interrupt_hook();
        bool skill_recharge_success_has_charged();
        bool skill_recharge_failure_couldnot_charge();
        
        //-------------------- Skill take_picture --------------------
        inline SkillState skill_take_picture_state() const { return skill_take_picture_state_; }
        virtual bool skill_take_picture_validate_hook();
        virtual void skill_take_picture_start_hook();
        virtual void skill_take_picture_on_start();
        virtual void skill_take_picture_interrupt_hook();
        bool skill_take_picture_success_pic_taken();
        bool skill_take_picture_failure_pic_failed();
        
    private:
        //-------------------- Skillset --------------------
        quadruped_example_skillset_interfaces::msg::SkillsetStatus status_() const;
        //-------------------- Event --------------------
        int event_set_standing_();
        int event_set_sitting_();
        int event_set_poweroff_();
        int event_set_poweron_();
        void skills_invariants_();
        //-------------------- Skill --------------------
        quadruped_example_skillset_interfaces::msg::SkillInitPowerResponse skill_init_power_response_initialize_() const;
        quadruped_example_skillset_interfaces::msg::SkillInitPowerResponse skill_init_power_preconditions_();
        quadruped_example_skillset_interfaces::msg::SkillInitPowerResponse skill_init_power_start_();
        quadruped_example_skillset_interfaces::msg::SkillInitPowerResponse skill_init_power_invariants_();
        quadruped_example_skillset_interfaces::msg::SkillInitPowerResponse skill_init_power_all_invariants_();
        void skill_init_power_interrupted_();quadruped_example_skillset_interfaces::msg::SkillSafePoweroffResponse skill_safe_poweroff_response_initialize_() const;
        quadruped_example_skillset_interfaces::msg::SkillSafePoweroffResponse skill_safe_poweroff_preconditions_();
        quadruped_example_skillset_interfaces::msg::SkillSafePoweroffResponse skill_safe_poweroff_start_();
        quadruped_example_skillset_interfaces::msg::SkillSafePoweroffResponse skill_safe_poweroff_invariants_();
        quadruped_example_skillset_interfaces::msg::SkillSafePoweroffResponse skill_safe_poweroff_all_invariants_();
        void skill_safe_poweroff_interrupted_();quadruped_example_skillset_interfaces::msg::SkillStandupResponse skill_standup_response_initialize_() const;
        quadruped_example_skillset_interfaces::msg::SkillStandupResponse skill_standup_preconditions_();
        quadruped_example_skillset_interfaces::msg::SkillStandupResponse skill_standup_start_();
        quadruped_example_skillset_interfaces::msg::SkillStandupResponse skill_standup_invariants_();
        quadruped_example_skillset_interfaces::msg::SkillStandupResponse skill_standup_all_invariants_();
        void skill_standup_interrupted_();quadruped_example_skillset_interfaces::msg::SkillSitdownResponse skill_sitdown_response_initialize_() const;
        quadruped_example_skillset_interfaces::msg::SkillSitdownResponse skill_sitdown_preconditions_();
        quadruped_example_skillset_interfaces::msg::SkillSitdownResponse skill_sitdown_start_();
        quadruped_example_skillset_interfaces::msg::SkillSitdownResponse skill_sitdown_invariants_();
        quadruped_example_skillset_interfaces::msg::SkillSitdownResponse skill_sitdown_all_invariants_();
        void skill_sitdown_interrupted_();quadruped_example_skillset_interfaces::msg::SkillGoToResponse skill_go_to_response_initialize_() const;
        quadruped_example_skillset_interfaces::msg::SkillGoToResponse skill_go_to_preconditions_();
        quadruped_example_skillset_interfaces::msg::SkillGoToResponse skill_go_to_start_();
        quadruped_example_skillset_interfaces::msg::SkillGoToResponse skill_go_to_invariants_();
        quadruped_example_skillset_interfaces::msg::SkillGoToResponse skill_go_to_all_invariants_();
        void skill_go_to_interrupted_();quadruped_example_skillset_interfaces::msg::SkillRechargeResponse skill_recharge_response_initialize_() const;
        quadruped_example_skillset_interfaces::msg::SkillRechargeResponse skill_recharge_preconditions_();
        quadruped_example_skillset_interfaces::msg::SkillRechargeResponse skill_recharge_start_();
        quadruped_example_skillset_interfaces::msg::SkillRechargeResponse skill_recharge_invariants_();
        quadruped_example_skillset_interfaces::msg::SkillRechargeResponse skill_recharge_all_invariants_();
        void skill_recharge_interrupted_();quadruped_example_skillset_interfaces::msg::SkillTakePictureResponse skill_take_picture_response_initialize_() const;
        quadruped_example_skillset_interfaces::msg::SkillTakePictureResponse skill_take_picture_preconditions_();
        quadruped_example_skillset_interfaces::msg::SkillTakePictureResponse skill_take_picture_start_();
        quadruped_example_skillset_interfaces::msg::SkillTakePictureResponse skill_take_picture_invariants_();
        quadruped_example_skillset_interfaces::msg::SkillTakePictureResponse skill_take_picture_all_invariants_();
        void skill_take_picture_interrupted_();
        //---------- Callback ----------
        void status_callback_(const std_msgs::msg::Empty::UniquePtr msg);
        // void data_callback_(const std_msgs::msg::String::UniquePtr msg);
        void event_callback_(const quadruped_example_skillset_interfaces::msg::EventRequest::UniquePtr msg);
        //---------- location ----------
        
        void data_location_request_callback_(const quadruped_example_skillset_interfaces::msg::DataRequest::UniquePtr msg);
        
        //---------- battery ----------
        
        void data_battery_request_callback_(const quadruped_example_skillset_interfaces::msg::DataRequest::UniquePtr msg);
        
        //---------- init_power ----------
        void skill_init_power_callback_(const quadruped_example_skillset_interfaces::msg::SkillInitPowerRequest::UniquePtr msg);
        void skill_init_power_interrupt_callback_(const quadruped_example_skillset_interfaces::msg::SkillInterrupt::UniquePtr msg);
        //---------- safe_poweroff ----------
        void skill_safe_poweroff_callback_(const quadruped_example_skillset_interfaces::msg::SkillSafePoweroffRequest::UniquePtr msg);
        void skill_safe_poweroff_interrupt_callback_(const quadruped_example_skillset_interfaces::msg::SkillInterrupt::UniquePtr msg);
        //---------- standup ----------
        void skill_standup_callback_(const quadruped_example_skillset_interfaces::msg::SkillStandupRequest::UniquePtr msg);
        void skill_standup_interrupt_callback_(const quadruped_example_skillset_interfaces::msg::SkillInterrupt::UniquePtr msg);
        //---------- sitdown ----------
        void skill_sitdown_callback_(const quadruped_example_skillset_interfaces::msg::SkillSitdownRequest::UniquePtr msg);
        void skill_sitdown_interrupt_callback_(const quadruped_example_skillset_interfaces::msg::SkillInterrupt::UniquePtr msg);
        //---------- go_to ----------
        void skill_go_to_callback_(const quadruped_example_skillset_interfaces::msg::SkillGoToRequest::UniquePtr msg);
        void skill_go_to_interrupt_callback_(const quadruped_example_skillset_interfaces::msg::SkillInterrupt::UniquePtr msg);
        //---------- recharge ----------
        void skill_recharge_callback_(const quadruped_example_skillset_interfaces::msg::SkillRechargeRequest::UniquePtr msg);
        void skill_recharge_interrupt_callback_(const quadruped_example_skillset_interfaces::msg::SkillInterrupt::UniquePtr msg);
        //---------- take_picture ----------
        void skill_take_picture_callback_(const quadruped_example_skillset_interfaces::msg::SkillTakePictureRequest::UniquePtr msg);
        void skill_take_picture_interrupt_callback_(const quadruped_example_skillset_interfaces::msg::SkillInterrupt::UniquePtr msg);

        std::mutex mutex_;
        std::string info_;

        //---------- Data ----------
        rclcpp::Time data_location_stamp_;
        std_msgs::msg::String data_location_;
        rclcpp::Publisher<quadruped_example_skillset_interfaces::msg::DataLocation>::SharedPtr data_location_pub_;
        
        rclcpp::Subscription<quadruped_example_skillset_interfaces::msg::DataRequest>::SharedPtr data_location_request_sub_;
        rclcpp::Publisher<quadruped_example_skillset_interfaces::msg::DataLocationResponse>::SharedPtr data_location_response_pub_;
        rclcpp::Time data_battery_stamp_;
        std_msgs::msg::Float64 data_battery_;
        rclcpp::Publisher<quadruped_example_skillset_interfaces::msg::DataBattery>::SharedPtr data_battery_pub_;
        
        rclcpp::Subscription<quadruped_example_skillset_interfaces::msg::DataRequest>::SharedPtr data_battery_request_sub_;
        rclcpp::Publisher<quadruped_example_skillset_interfaces::msg::DataBatteryResponse>::SharedPtr data_battery_response_pub_;
        
        //---------- Resource ----------
        std::shared_ptr<MotorState> resource_motor_state_;
        std::shared_ptr<SpotState> resource_spot_state_;
        
        //---------- Topics ----------
        rclcpp::Subscription<std_msgs::msg::Empty>::SharedPtr status_sub_;
        // rclcpp::Subscription<std_msgs::msg::String>::SharedPtr data_sub_;
        rclcpp::Subscription<quadruped_example_skillset_interfaces::msg::EventRequest>::SharedPtr event_sub_;
        //
        rclcpp::Publisher<quadruped_example_skillset_interfaces::msg::SkillsetStatus>::SharedPtr status_pub_;
        rclcpp::Publisher<quadruped_example_skillset_interfaces::msg::EventResponse>::SharedPtr event_pub_;
        
        //---------- init_power ----------
        SkillState skill_init_power_state_;
        std::string skill_init_power_id_;
        rclcpp::Subscription<quadruped_example_skillset_interfaces::msg::SkillInitPowerRequest>::SharedPtr skill_init_power_request_sub_;
        rclcpp::Subscription<quadruped_example_skillset_interfaces::msg::SkillInterrupt>::SharedPtr skill_init_power_interrupt_sub_;
        rclcpp::Publisher<quadruped_example_skillset_interfaces::msg::SkillInitPowerResponse>::SharedPtr skill_init_power_response_pub_;
        //---------- safe_poweroff ----------
        SkillState skill_safe_poweroff_state_;
        std::string skill_safe_poweroff_id_;
        rclcpp::Subscription<quadruped_example_skillset_interfaces::msg::SkillSafePoweroffRequest>::SharedPtr skill_safe_poweroff_request_sub_;
        rclcpp::Subscription<quadruped_example_skillset_interfaces::msg::SkillInterrupt>::SharedPtr skill_safe_poweroff_interrupt_sub_;
        rclcpp::Publisher<quadruped_example_skillset_interfaces::msg::SkillSafePoweroffResponse>::SharedPtr skill_safe_poweroff_response_pub_;
        //---------- standup ----------
        SkillState skill_standup_state_;
        std::string skill_standup_id_;
        rclcpp::Subscription<quadruped_example_skillset_interfaces::msg::SkillStandupRequest>::SharedPtr skill_standup_request_sub_;
        rclcpp::Subscription<quadruped_example_skillset_interfaces::msg::SkillInterrupt>::SharedPtr skill_standup_interrupt_sub_;
        rclcpp::Publisher<quadruped_example_skillset_interfaces::msg::SkillStandupResponse>::SharedPtr skill_standup_response_pub_;
        //---------- sitdown ----------
        SkillState skill_sitdown_state_;
        std::string skill_sitdown_id_;
        rclcpp::Subscription<quadruped_example_skillset_interfaces::msg::SkillSitdownRequest>::SharedPtr skill_sitdown_request_sub_;
        rclcpp::Subscription<quadruped_example_skillset_interfaces::msg::SkillInterrupt>::SharedPtr skill_sitdown_interrupt_sub_;
        rclcpp::Publisher<quadruped_example_skillset_interfaces::msg::SkillSitdownResponse>::SharedPtr skill_sitdown_response_pub_;
        //---------- go_to ----------
        SkillState skill_go_to_state_;
        std::string skill_go_to_id_;
        quadruped_example_skillset_interfaces::msg::SkillGoToInput::SharedPtr skill_go_to_input_;
        rclcpp::Subscription<quadruped_example_skillset_interfaces::msg::SkillGoToRequest>::SharedPtr skill_go_to_request_sub_;
        rclcpp::Subscription<quadruped_example_skillset_interfaces::msg::SkillInterrupt>::SharedPtr skill_go_to_interrupt_sub_;
        rclcpp::Publisher<quadruped_example_skillset_interfaces::msg::SkillGoToResponse>::SharedPtr skill_go_to_response_pub_;
        //---------- recharge ----------
        SkillState skill_recharge_state_;
        std::string skill_recharge_id_;
        rclcpp::Subscription<quadruped_example_skillset_interfaces::msg::SkillRechargeRequest>::SharedPtr skill_recharge_request_sub_;
        rclcpp::Subscription<quadruped_example_skillset_interfaces::msg::SkillInterrupt>::SharedPtr skill_recharge_interrupt_sub_;
        rclcpp::Publisher<quadruped_example_skillset_interfaces::msg::SkillRechargeResponse>::SharedPtr skill_recharge_response_pub_;
        //---------- take_picture ----------
        SkillState skill_take_picture_state_;
        std::string skill_take_picture_id_;
        rclcpp::Subscription<quadruped_example_skillset_interfaces::msg::SkillTakePictureRequest>::SharedPtr skill_take_picture_request_sub_;
        rclcpp::Subscription<quadruped_example_skillset_interfaces::msg::SkillInterrupt>::SharedPtr skill_take_picture_interrupt_sub_;
        rclcpp::Publisher<quadruped_example_skillset_interfaces::msg::SkillTakePictureResponse>::SharedPtr skill_take_picture_response_pub_;
    };
}
#endif
