#include "quadruped_example_skillset/NodeDebug.hpp"

namespace quadruped_example_skillset
{

    QuadrupedExampleNodeDebug::QuadrupedExampleNodeDebug(const std::string node_name, const std::string info) : QuadrupedExampleNode(node_name, info)
    {
        
        //---------- Resource motor_state ----------
        this->declare_parameter("resource_motor_state", "Off");
        resource_motor_state_handler_ = std::make_shared<rclcpp::ParameterEventHandler>(this);
        resource_motor_state_handle_ = resource_motor_state_handler_->add_parameter_callback("resource_motor_state",
            [this](const rclcpp::Parameter &parameter)
            {
                    this->resource_motor_state_hook_(parameter);
            });
        
        //---------- Resource spot_state ----------
        this->declare_parameter("resource_spot_state", "Sitting");
        resource_spot_state_handler_ = std::make_shared<rclcpp::ParameterEventHandler>(this);
        resource_spot_state_handle_ = resource_spot_state_handler_->add_parameter_callback("resource_spot_state",
            [this](const rclcpp::Parameter &parameter)
            {
                    this->resource_spot_state_hook_(parameter);
            });
        
    }

    
    void QuadrupedExampleNodeDebug::resource_motor_state_hook_(const rclcpp::Parameter &parameter)
    {
        auto state = parameter.as_string();
        
        if (state == "Off")
        {
            this->resource_motor_state_->set_next(MotorStateState::Off);
            // status
            auto status_message = status_();
            status_pub_->publish(status_message);
        }
        else if (state == "On")
        {
            this->resource_motor_state_->set_next(MotorStateState::On);
            // status
            auto status_message = status_();
            status_pub_->publish(status_message);
        }
        else
        {
            RCLCPP_WARN(this->get_logger(), "resource 'motor_state' state '%s' undefined", state.c_str());
        }
    }
    
    void QuadrupedExampleNodeDebug::resource_spot_state_hook_(const rclcpp::Parameter &parameter)
    {
        auto state = parameter.as_string();
        
        if (state == "Sitting")
        {
            this->resource_spot_state_->set_next(SpotStateState::Sitting);
            // status
            auto status_message = status_();
            status_pub_->publish(status_message);
        }
        else if (state == "Standing")
        {
            this->resource_spot_state_->set_next(SpotStateState::Standing);
            // status
            auto status_message = status_();
            status_pub_->publish(status_message);
        }
        else
        {
            RCLCPP_WARN(this->get_logger(), "resource 'spot_state' state '%s' undefined", state.c_str());
        }
    }
    
}
