#ifndef QUADRUPED_EXAMPLE_NODE_DEBUG_HPP
#define QUADRUPED_EXAMPLE_NODE_DEBUG_HPP

#include "Node.hpp"

namespace quadruped_example_skillset
{
    class QuadrupedExampleNodeDebug: public QuadrupedExampleNode
    {
    public:
        QuadrupedExampleNodeDebug(const std::string node_name, const std::string info);
        virtual ~QuadrupedExampleNodeDebug() {}

    private:
        
        void resource_motor_state_hook_(const rclcpp::Parameter &parameter);
        std::shared_ptr<rclcpp::ParameterEventHandler> resource_motor_state_handler_;
        std::shared_ptr<rclcpp::ParameterCallbackHandle> resource_motor_state_handle_;
        
        void resource_spot_state_hook_(const rclcpp::Parameter &parameter);
        std::shared_ptr<rclcpp::ParameterEventHandler> resource_spot_state_handler_;
        std::shared_ptr<rclcpp::ParameterCallbackHandle> resource_spot_state_handle_;
        
    };
}
#endif
