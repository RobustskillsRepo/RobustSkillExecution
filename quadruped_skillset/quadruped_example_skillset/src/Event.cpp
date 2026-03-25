#include "quadruped_example_skillset/Node.hpp"

namespace quadruped_example_skillset
{

    void QuadrupedExampleNode::event_callback_(const quadruped_example_skillset_interfaces::msg::EventRequest::UniquePtr msg)
    {
        mutex_.lock();
        RCLCPP_DEBUG(this->get_logger(), "skillset 'quadruped_example' event '%s' request", msg->name.c_str());
        auto message = quadruped_example_skillset_interfaces::msg::EventResponse();
        message.id = msg->id;
        message.response = quadruped_example_skillset_interfaces::msg::EventResponse::UNDEFINED;
        
        if (msg->name == "set_standing") {
            message.response = event_set_standing_();
        }
        
        else if (msg->name == "set_sitting") {
            message.response = event_set_sitting_();
        }
        
        else if (msg->name == "set_poweroff") {
            message.response = event_set_poweroff_();
        }
        
        else if (msg->name == "set_poweron") {
            message.response = event_set_poweron_();
        }
        
        event_pub_->publish(message);
        // status
        auto status_message = status_();
        status_pub_->publish(status_message);
        mutex_.unlock();
    }

    
    //-------------------------------------------------- set_standing --------------------------------------------------

    int QuadrupedExampleNode::event_set_standing_()
    {
        // guard
        
        if (!((resource_spot_state_->current() == SpotStateState::Sitting)))
        {
            return quadruped_example_skillset_interfaces::msg::EventResponse::GUARD_FAILURE;
        }
        
        
        // check effects
        if (!(
             resource_spot_state_->check_next(SpotStateState::Standing)
        ))
        {
            return quadruped_example_skillset_interfaces::msg::EventResponse::EFFECT_FAILURE;
        }
        
        // hook
        event_set_standing_hook();
        // set effects
        resource_spot_state_->set_next(SpotStateState::Standing);
        
        skills_invariants_();
        return quadruped_example_skillset_interfaces::msg::EventResponse::SUCCESS;
    }

    void QuadrupedExampleNode::event_set_standing_hook()
    {
    }

    void QuadrupedExampleNode::event_set_standing()
    {
        mutex_.lock();
        RCLCPP_DEBUG(this->get_logger(), "skillset event 'set_standing'");
        auto message = quadruped_example_skillset_interfaces::msg::EventResponse();
        message.id = info_;
        message.response = event_set_standing_();
        event_pub_->publish(message);
        // status
        auto status_message = status_();
        status_pub_->publish(status_message);
        mutex_.unlock();
    }

    
    //-------------------------------------------------- set_sitting --------------------------------------------------

    int QuadrupedExampleNode::event_set_sitting_()
    {
        // guard
        
        if (!((resource_spot_state_->current() == SpotStateState::Standing)))
        {
            return quadruped_example_skillset_interfaces::msg::EventResponse::GUARD_FAILURE;
        }
        
        
        // check effects
        if (!(
             resource_spot_state_->check_next(SpotStateState::Sitting)
        ))
        {
            return quadruped_example_skillset_interfaces::msg::EventResponse::EFFECT_FAILURE;
        }
        
        // hook
        event_set_sitting_hook();
        // set effects
        resource_spot_state_->set_next(SpotStateState::Sitting);
        
        skills_invariants_();
        return quadruped_example_skillset_interfaces::msg::EventResponse::SUCCESS;
    }

    void QuadrupedExampleNode::event_set_sitting_hook()
    {
    }

    void QuadrupedExampleNode::event_set_sitting()
    {
        mutex_.lock();
        RCLCPP_DEBUG(this->get_logger(), "skillset event 'set_sitting'");
        auto message = quadruped_example_skillset_interfaces::msg::EventResponse();
        message.id = info_;
        message.response = event_set_sitting_();
        event_pub_->publish(message);
        // status
        auto status_message = status_();
        status_pub_->publish(status_message);
        mutex_.unlock();
    }

    
    //-------------------------------------------------- set_poweroff --------------------------------------------------

    int QuadrupedExampleNode::event_set_poweroff_()
    {
        // guard
        
        if (!((resource_motor_state_->current() == MotorStateState::On)))
        {
            return quadruped_example_skillset_interfaces::msg::EventResponse::GUARD_FAILURE;
        }
        
        
        // check effects
        if (!(
             resource_spot_state_->check_next(SpotStateState::Sitting)
             &&  resource_motor_state_->check_next(MotorStateState::Off)
        ))
        {
            return quadruped_example_skillset_interfaces::msg::EventResponse::EFFECT_FAILURE;
        }
        
        // hook
        event_set_poweroff_hook();
        // set effects
        resource_spot_state_->set_next(SpotStateState::Sitting);
        resource_motor_state_->set_next(MotorStateState::Off);
        
        skills_invariants_();
        return quadruped_example_skillset_interfaces::msg::EventResponse::SUCCESS;
    }

    void QuadrupedExampleNode::event_set_poweroff_hook()
    {
    }

    void QuadrupedExampleNode::event_set_poweroff()
    {
        mutex_.lock();
        RCLCPP_DEBUG(this->get_logger(), "skillset event 'set_poweroff'");
        auto message = quadruped_example_skillset_interfaces::msg::EventResponse();
        message.id = info_;
        message.response = event_set_poweroff_();
        event_pub_->publish(message);
        // status
        auto status_message = status_();
        status_pub_->publish(status_message);
        mutex_.unlock();
    }

    
    //-------------------------------------------------- set_poweron --------------------------------------------------

    int QuadrupedExampleNode::event_set_poweron_()
    {
        // guard
        
        if (!((resource_motor_state_->current() == MotorStateState::Off)))
        {
            return quadruped_example_skillset_interfaces::msg::EventResponse::GUARD_FAILURE;
        }
        
        
        // check effects
        if (!(
             resource_motor_state_->check_next(MotorStateState::On)
        ))
        {
            return quadruped_example_skillset_interfaces::msg::EventResponse::EFFECT_FAILURE;
        }
        
        // hook
        event_set_poweron_hook();
        // set effects
        resource_motor_state_->set_next(MotorStateState::On);
        
        skills_invariants_();
        return quadruped_example_skillset_interfaces::msg::EventResponse::SUCCESS;
    }

    void QuadrupedExampleNode::event_set_poweron_hook()
    {
    }

    void QuadrupedExampleNode::event_set_poweron()
    {
        mutex_.lock();
        RCLCPP_DEBUG(this->get_logger(), "skillset event 'set_poweron'");
        auto message = quadruped_example_skillset_interfaces::msg::EventResponse();
        message.id = info_;
        message.response = event_set_poweron_();
        event_pub_->publish(message);
        // status
        auto status_message = status_();
        status_pub_->publish(status_message);
        mutex_.unlock();
    }

    
}
