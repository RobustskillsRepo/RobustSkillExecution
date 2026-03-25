#include "quadruped_example_skillset/Node.hpp"

namespace quadruped_example_skillset
{

    QuadrupedExampleNode::QuadrupedExampleNode(const std::string node_name, const std::string info)
        : Node(node_name)
        , info_(info)
        , data_location_stamp_(0)
        , data_battery_stamp_(0)
        , resource_motor_state_(std::make_shared<MotorState>())
        , resource_spot_state_(std::make_shared<SpotState>())
        , skill_init_power_state_(SkillState::Ready)
        , skill_init_power_id_("")
        
        , skill_safe_poweroff_state_(SkillState::Ready)
        , skill_safe_poweroff_id_("")
        
        , skill_standup_state_(SkillState::Ready)
        , skill_standup_id_("")
        
        , skill_sitdown_state_(SkillState::Ready)
        , skill_sitdown_id_("")
        
        , skill_go_to_state_(SkillState::Ready)
        , skill_go_to_id_("")
        , skill_go_to_input_(std::make_shared<quadruped_example_skillset_interfaces::msg::SkillGoToInput>()) 
        , skill_recharge_state_(SkillState::Ready)
        , skill_recharge_id_("")
        
        , skill_take_picture_state_(SkillState::Ready)
        , skill_take_picture_id_("")
        
        {
        //-------------------- QoS --------------------
        auto qos_soft = rclcpp::QoS(1).best_effort().keep_last(1).durability_volatile();
        auto qos_hard = rclcpp::QoS(1).reliable().keep_last(1).transient_local();
        auto qos_event = rclcpp::QoS(1).reliable().keep_all().durability_volatile();
        //-------------------- Skillset --------------------
        status_sub_ = this->create_subscription<std_msgs::msg::Empty>(
            "~/quadruped_example_skillset/status_request", qos_event, 
            [this](std_msgs::msg::Empty::UniquePtr msg) { 
                this->QuadrupedExampleNode::status_callback_(std::move(msg)); 
            });
        event_sub_ = this->create_subscription<quadruped_example_skillset_interfaces::msg::EventRequest>(
            "~/quadruped_example_skillset/event_request", qos_event, 
            [this](quadruped_example_skillset_interfaces::msg::EventRequest::UniquePtr msg) {
                this->QuadrupedExampleNode::event_callback_(std::move(msg));
            });
        status_pub_ = this->create_publisher<quadruped_example_skillset_interfaces::msg::SkillsetStatus>(
            "~/quadruped_example_skillset/status", qos_event);
        event_pub_ = this->create_publisher<quadruped_example_skillset_interfaces::msg::EventResponse>(
            "~/quadruped_example_skillset/event_response", qos_event);
        //-------------------- Data --------------------
        //---------- Data location ----------
        data_location_pub_ = this->create_publisher<quadruped_example_skillset_interfaces::msg::DataLocation>(
            "~/quadruped_example_skillset/data/location", qos_hard);
        
        data_location_request_sub_ = this->create_subscription<quadruped_example_skillset_interfaces::msg::DataRequest>(
            "~/quadruped_example_skillset/data/location/request", qos_event, 
            [this](quadruped_example_skillset_interfaces::msg::DataRequest::UniquePtr msg) { 
                this->QuadrupedExampleNode::data_location_request_callback_(std::move(msg)); 
            });
        data_location_response_pub_ = this->create_publisher<quadruped_example_skillset_interfaces::msg::DataLocationResponse>(
            "~/quadruped_example_skillset/data/location/response", qos_event);
        //---------- Data battery ----------
        data_battery_pub_ = this->create_publisher<quadruped_example_skillset_interfaces::msg::DataBattery>(
            "~/quadruped_example_skillset/data/battery", qos_hard);
        
        data_battery_request_sub_ = this->create_subscription<quadruped_example_skillset_interfaces::msg::DataRequest>(
            "~/quadruped_example_skillset/data/battery/request", qos_event, 
            [this](quadruped_example_skillset_interfaces::msg::DataRequest::UniquePtr msg) { 
                this->QuadrupedExampleNode::data_battery_request_callback_(std::move(msg)); 
            });
        data_battery_response_pub_ = this->create_publisher<quadruped_example_skillset_interfaces::msg::DataBatteryResponse>(
            "~/quadruped_example_skillset/data/battery/response", qos_event);
        
        //-------------------- Skill --------------------
        
        //---------- Skill init_power ----------
        skill_init_power_request_sub_ = this->create_subscription<quadruped_example_skillset_interfaces::msg::SkillInitPowerRequest>(
            "~/quadruped_example_skillset/skill/init_power/request", qos_event, 
            [this](quadruped_example_skillset_interfaces::msg::SkillInitPowerRequest::UniquePtr msg) { 
                this->QuadrupedExampleNode::skill_init_power_callback_(std::move(msg)); 
            });
        skill_init_power_response_pub_ = this->create_publisher<quadruped_example_skillset_interfaces::msg::SkillInitPowerResponse>(
            "~/quadruped_example_skillset/skill/init_power/response", qos_event);
        
        skill_init_power_interrupt_sub_ = this->create_subscription<quadruped_example_skillset_interfaces::msg::SkillInterrupt>(
            "~/quadruped_example_skillset/skill/init_power/interrupt", qos_event, 
            [this](quadruped_example_skillset_interfaces::msg::SkillInterrupt::UniquePtr msg) { 
                this->QuadrupedExampleNode::skill_init_power_interrupt_callback_(std::move(msg)); 
            });
        
        //---------- Skill safe_poweroff ----------
        skill_safe_poweroff_request_sub_ = this->create_subscription<quadruped_example_skillset_interfaces::msg::SkillSafePoweroffRequest>(
            "~/quadruped_example_skillset/skill/safe_poweroff/request", qos_event, 
            [this](quadruped_example_skillset_interfaces::msg::SkillSafePoweroffRequest::UniquePtr msg) { 
                this->QuadrupedExampleNode::skill_safe_poweroff_callback_(std::move(msg)); 
            });
        skill_safe_poweroff_response_pub_ = this->create_publisher<quadruped_example_skillset_interfaces::msg::SkillSafePoweroffResponse>(
            "~/quadruped_example_skillset/skill/safe_poweroff/response", qos_event);
        
        skill_safe_poweroff_interrupt_sub_ = this->create_subscription<quadruped_example_skillset_interfaces::msg::SkillInterrupt>(
            "~/quadruped_example_skillset/skill/safe_poweroff/interrupt", qos_event, 
            [this](quadruped_example_skillset_interfaces::msg::SkillInterrupt::UniquePtr msg) { 
                this->QuadrupedExampleNode::skill_safe_poweroff_interrupt_callback_(std::move(msg)); 
            });
        
        //---------- Skill standup ----------
        skill_standup_request_sub_ = this->create_subscription<quadruped_example_skillset_interfaces::msg::SkillStandupRequest>(
            "~/quadruped_example_skillset/skill/standup/request", qos_event, 
            [this](quadruped_example_skillset_interfaces::msg::SkillStandupRequest::UniquePtr msg) { 
                this->QuadrupedExampleNode::skill_standup_callback_(std::move(msg)); 
            });
        skill_standup_response_pub_ = this->create_publisher<quadruped_example_skillset_interfaces::msg::SkillStandupResponse>(
            "~/quadruped_example_skillset/skill/standup/response", qos_event);
        
        skill_standup_interrupt_sub_ = this->create_subscription<quadruped_example_skillset_interfaces::msg::SkillInterrupt>(
            "~/quadruped_example_skillset/skill/standup/interrupt", qos_event, 
            [this](quadruped_example_skillset_interfaces::msg::SkillInterrupt::UniquePtr msg) { 
                this->QuadrupedExampleNode::skill_standup_interrupt_callback_(std::move(msg)); 
            });
        
        //---------- Skill sitdown ----------
        skill_sitdown_request_sub_ = this->create_subscription<quadruped_example_skillset_interfaces::msg::SkillSitdownRequest>(
            "~/quadruped_example_skillset/skill/sitdown/request", qos_event, 
            [this](quadruped_example_skillset_interfaces::msg::SkillSitdownRequest::UniquePtr msg) { 
                this->QuadrupedExampleNode::skill_sitdown_callback_(std::move(msg)); 
            });
        skill_sitdown_response_pub_ = this->create_publisher<quadruped_example_skillset_interfaces::msg::SkillSitdownResponse>(
            "~/quadruped_example_skillset/skill/sitdown/response", qos_event);
        
        skill_sitdown_interrupt_sub_ = this->create_subscription<quadruped_example_skillset_interfaces::msg::SkillInterrupt>(
            "~/quadruped_example_skillset/skill/sitdown/interrupt", qos_event, 
            [this](quadruped_example_skillset_interfaces::msg::SkillInterrupt::UniquePtr msg) { 
                this->QuadrupedExampleNode::skill_sitdown_interrupt_callback_(std::move(msg)); 
            });
        
        //---------- Skill go_to ----------
        skill_go_to_request_sub_ = this->create_subscription<quadruped_example_skillset_interfaces::msg::SkillGoToRequest>(
            "~/quadruped_example_skillset/skill/go_to/request", qos_event, 
            [this](quadruped_example_skillset_interfaces::msg::SkillGoToRequest::UniquePtr msg) { 
                this->QuadrupedExampleNode::skill_go_to_callback_(std::move(msg)); 
            });
        skill_go_to_response_pub_ = this->create_publisher<quadruped_example_skillset_interfaces::msg::SkillGoToResponse>(
            "~/quadruped_example_skillset/skill/go_to/response", qos_event);
        
        skill_go_to_interrupt_sub_ = this->create_subscription<quadruped_example_skillset_interfaces::msg::SkillInterrupt>(
            "~/quadruped_example_skillset/skill/go_to/interrupt", qos_event, 
            [this](quadruped_example_skillset_interfaces::msg::SkillInterrupt::UniquePtr msg) { 
                this->QuadrupedExampleNode::skill_go_to_interrupt_callback_(std::move(msg)); 
            });
        
        //---------- Skill recharge ----------
        skill_recharge_request_sub_ = this->create_subscription<quadruped_example_skillset_interfaces::msg::SkillRechargeRequest>(
            "~/quadruped_example_skillset/skill/recharge/request", qos_event, 
            [this](quadruped_example_skillset_interfaces::msg::SkillRechargeRequest::UniquePtr msg) { 
                this->QuadrupedExampleNode::skill_recharge_callback_(std::move(msg)); 
            });
        skill_recharge_response_pub_ = this->create_publisher<quadruped_example_skillset_interfaces::msg::SkillRechargeResponse>(
            "~/quadruped_example_skillset/skill/recharge/response", qos_event);
        
        skill_recharge_interrupt_sub_ = this->create_subscription<quadruped_example_skillset_interfaces::msg::SkillInterrupt>(
            "~/quadruped_example_skillset/skill/recharge/interrupt", qos_event, 
            [this](quadruped_example_skillset_interfaces::msg::SkillInterrupt::UniquePtr msg) { 
                this->QuadrupedExampleNode::skill_recharge_interrupt_callback_(std::move(msg)); 
            });
        
        //---------- Skill take_picture ----------
        skill_take_picture_request_sub_ = this->create_subscription<quadruped_example_skillset_interfaces::msg::SkillTakePictureRequest>(
            "~/quadruped_example_skillset/skill/take_picture/request", qos_event, 
            [this](quadruped_example_skillset_interfaces::msg::SkillTakePictureRequest::UniquePtr msg) { 
                this->QuadrupedExampleNode::skill_take_picture_callback_(std::move(msg)); 
            });
        skill_take_picture_response_pub_ = this->create_publisher<quadruped_example_skillset_interfaces::msg::SkillTakePictureResponse>(
            "~/quadruped_example_skillset/skill/take_picture/response", qos_event);
        
        skill_take_picture_interrupt_sub_ = this->create_subscription<quadruped_example_skillset_interfaces::msg::SkillInterrupt>(
            "~/quadruped_example_skillset/skill/take_picture/interrupt", qos_event, 
            [this](quadruped_example_skillset_interfaces::msg::SkillInterrupt::UniquePtr msg) { 
                this->QuadrupedExampleNode::skill_take_picture_interrupt_callback_(std::move(msg)); 
            });
        
    }

    quadruped_example_skillset_interfaces::msg::SkillsetStatus QuadrupedExampleNode::status_() const {
        auto message = quadruped_example_skillset_interfaces::msg::SkillsetStatus();
        // Stamp
        message.stamp = rclcpp::Node::now();
        // resources
        auto rs = quadruped_example_skillset_interfaces::msg::ResourceState();
        
        rs = quadruped_example_skillset_interfaces::msg::ResourceState();
        rs.name = "motor_state";
        rs.state = to_string(resource_motor_state_->current());
        message.resources.push_back(rs);
        
        rs = quadruped_example_skillset_interfaces::msg::ResourceState();
        rs.name = "spot_state";
        rs.state = to_string(resource_spot_state_->current());
        message.resources.push_back(rs);
        
        // skills
        
        message.skill_init_power = quadruped_example_skillset_interfaces::msg::SkillInitPowerStatus();
        message.skill_init_power.name = "init_power";
        message.skill_init_power.id = skill_init_power_id_;
        switch (skill_init_power_state_)
        {
        case SkillState::Ready:
            message.skill_init_power.state = quadruped_example_skillset_interfaces::msg::SkillInitPowerStatus::READY;
            break;        
        case SkillState::Running:
            message.skill_init_power.state = quadruped_example_skillset_interfaces::msg::SkillInitPowerStatus::RUNNING;
            break;        
        case SkillState::Interrupting:
            message.skill_init_power.state = quadruped_example_skillset_interfaces::msg::SkillInitPowerStatus::INTERRUPTING;
            break;        
        default:
            break;
        }
        
        
        message.skill_safe_poweroff = quadruped_example_skillset_interfaces::msg::SkillSafePoweroffStatus();
        message.skill_safe_poweroff.name = "safe_poweroff";
        message.skill_safe_poweroff.id = skill_safe_poweroff_id_;
        switch (skill_safe_poweroff_state_)
        {
        case SkillState::Ready:
            message.skill_safe_poweroff.state = quadruped_example_skillset_interfaces::msg::SkillSafePoweroffStatus::READY;
            break;        
        case SkillState::Running:
            message.skill_safe_poweroff.state = quadruped_example_skillset_interfaces::msg::SkillSafePoweroffStatus::RUNNING;
            break;        
        case SkillState::Interrupting:
            message.skill_safe_poweroff.state = quadruped_example_skillset_interfaces::msg::SkillSafePoweroffStatus::INTERRUPTING;
            break;        
        default:
            break;
        }
        
        
        message.skill_standup = quadruped_example_skillset_interfaces::msg::SkillStandupStatus();
        message.skill_standup.name = "standup";
        message.skill_standup.id = skill_standup_id_;
        switch (skill_standup_state_)
        {
        case SkillState::Ready:
            message.skill_standup.state = quadruped_example_skillset_interfaces::msg::SkillStandupStatus::READY;
            break;        
        case SkillState::Running:
            message.skill_standup.state = quadruped_example_skillset_interfaces::msg::SkillStandupStatus::RUNNING;
            break;        
        case SkillState::Interrupting:
            message.skill_standup.state = quadruped_example_skillset_interfaces::msg::SkillStandupStatus::INTERRUPTING;
            break;        
        default:
            break;
        }
        
        
        message.skill_sitdown = quadruped_example_skillset_interfaces::msg::SkillSitdownStatus();
        message.skill_sitdown.name = "sitdown";
        message.skill_sitdown.id = skill_sitdown_id_;
        switch (skill_sitdown_state_)
        {
        case SkillState::Ready:
            message.skill_sitdown.state = quadruped_example_skillset_interfaces::msg::SkillSitdownStatus::READY;
            break;        
        case SkillState::Running:
            message.skill_sitdown.state = quadruped_example_skillset_interfaces::msg::SkillSitdownStatus::RUNNING;
            break;        
        case SkillState::Interrupting:
            message.skill_sitdown.state = quadruped_example_skillset_interfaces::msg::SkillSitdownStatus::INTERRUPTING;
            break;        
        default:
            break;
        }
        
        
        message.skill_go_to = quadruped_example_skillset_interfaces::msg::SkillGoToStatus();
        message.skill_go_to.name = "go_to";
        message.skill_go_to.id = skill_go_to_id_;
        switch (skill_go_to_state_)
        {
        case SkillState::Ready:
            message.skill_go_to.state = quadruped_example_skillset_interfaces::msg::SkillGoToStatus::READY;
            break;        
        case SkillState::Running:
            message.skill_go_to.state = quadruped_example_skillset_interfaces::msg::SkillGoToStatus::RUNNING;
            break;        
        case SkillState::Interrupting:
            message.skill_go_to.state = quadruped_example_skillset_interfaces::msg::SkillGoToStatus::INTERRUPTING;
            break;        
        default:
            break;
        }
        message.skill_go_to.input = *skill_go_to_input_;
        
        message.skill_recharge = quadruped_example_skillset_interfaces::msg::SkillRechargeStatus();
        message.skill_recharge.name = "recharge";
        message.skill_recharge.id = skill_recharge_id_;
        switch (skill_recharge_state_)
        {
        case SkillState::Ready:
            message.skill_recharge.state = quadruped_example_skillset_interfaces::msg::SkillRechargeStatus::READY;
            break;        
        case SkillState::Running:
            message.skill_recharge.state = quadruped_example_skillset_interfaces::msg::SkillRechargeStatus::RUNNING;
            break;        
        case SkillState::Interrupting:
            message.skill_recharge.state = quadruped_example_skillset_interfaces::msg::SkillRechargeStatus::INTERRUPTING;
            break;        
        default:
            break;
        }
        
        
        message.skill_take_picture = quadruped_example_skillset_interfaces::msg::SkillTakePictureStatus();
        message.skill_take_picture.name = "take_picture";
        message.skill_take_picture.id = skill_take_picture_id_;
        switch (skill_take_picture_state_)
        {
        case SkillState::Ready:
            message.skill_take_picture.state = quadruped_example_skillset_interfaces::msg::SkillTakePictureStatus::READY;
            break;        
        case SkillState::Running:
            message.skill_take_picture.state = quadruped_example_skillset_interfaces::msg::SkillTakePictureStatus::RUNNING;
            break;        
        case SkillState::Interrupting:
            message.skill_take_picture.state = quadruped_example_skillset_interfaces::msg::SkillTakePictureStatus::INTERRUPTING;
            break;        
        default:
            break;
        }
        
        
        // Info
        message.info = info_;
        return message;
    }

    void QuadrupedExampleNode::status_callback_(const std_msgs::msg::Empty::UniquePtr msg)
    {
        mutex_.lock();
        (void)msg;
        RCLCPP_DEBUG(this->get_logger(), "skillset 'quadruped_example' status request");
        auto message = status_();
        status_pub_->publish(message);
        mutex_.unlock();
    }

    //-------------------------------------------------- Resources --------------------------------------------------
    
    std::string QuadrupedExampleNode::get_motor_state_state()
    {
        mutex_.lock();
        std::string state = to_string(resource_motor_state_->current());
        mutex_.unlock();
        return state;
    }
    std::string QuadrupedExampleNode::get_motor_state_state_hook()
    {
        std::string state = to_string(resource_motor_state_->current());
        return state;
    }
    
    std::string QuadrupedExampleNode::get_spot_state_state()
    {
        mutex_.lock();
        std::string state = to_string(resource_spot_state_->current());
        mutex_.unlock();
        return state;
    }
    std::string QuadrupedExampleNode::get_spot_state_state_hook()
    {
        std::string state = to_string(resource_spot_state_->current());
        return state;
    }
    

    //-------------------------------------------------- Skillset Status --------------------------------------------------
    quadruped_example_skillset_interfaces::msg::SkillsetStatus QuadrupedExampleNode::get_skillset_status()
    {
        mutex_.lock();
        auto status = this->status_();
        mutex_.unlock();
        return status;
    }
}
