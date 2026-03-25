#include "quadruped_example_skillset/Node.hpp"

namespace quadruped_example_skillset
{
    
    //-------------------------------------------------- location --------------------------------------------------

    quadruped_example_skillset_interfaces::msg::DataLocation QuadrupedExampleNode::get_data_location()
    {
        quadruped_example_skillset_interfaces::msg::DataLocation message;
        mutex_.lock();
        message.stamp = data_location_stamp_;
        message.value = data_location_;
        mutex_.unlock();
        return message;
    }

    void QuadrupedExampleNode::set_data_location(std_msgs::msg::String data)
    {
        mutex_.lock();
        // Data
        data_location_ = data;
        data_location_stamp_ = rclcpp::Node::now();
        // Message
        quadruped_example_skillset_interfaces::msg::DataLocation message;
        message.stamp = data_location_stamp_;
        message.value = data_location_;
        data_location_pub_->publish(message);
        mutex_.unlock();
    }

    quadruped_example_skillset_interfaces::msg::DataLocation QuadrupedExampleNode::get_data_location_hook()
    {
        quadruped_example_skillset_interfaces::msg::DataLocation message;
        message.stamp = data_location_stamp_;
        message.value = data_location_;
        return message;
    }

    void QuadrupedExampleNode::set_data_location_hook(std_msgs::msg::String data)
    {
        // Data
        data_location_ = data;
        data_location_stamp_ = rclcpp::Node::now();
        // Message
        quadruped_example_skillset_interfaces::msg::DataLocation message;
        message.stamp = data_location_stamp_;
        message.value = data_location_;
        data_location_pub_->publish(message);
    }

    

    void QuadrupedExampleNode::data_location_request_callback_(const quadruped_example_skillset_interfaces::msg::DataRequest::UniquePtr msg)
    {
        quadruped_example_skillset_interfaces::msg::DataLocationResponse message;
        message.id = msg->id;
        mutex_.lock();
        if (data_location_stamp_.nanoseconds() > 0) {
            message.has_data = true;
            message.stamp = data_location_stamp_;
            message.value = data_location_;
        }
        else {
            message.has_data = false;
            message.stamp = data_location_stamp_;
        }
        mutex_.unlock();
        data_location_response_pub_->publish(message);
    }

    
    //-------------------------------------------------- battery --------------------------------------------------

    quadruped_example_skillset_interfaces::msg::DataBattery QuadrupedExampleNode::get_data_battery()
    {
        quadruped_example_skillset_interfaces::msg::DataBattery message;
        mutex_.lock();
        message.stamp = data_battery_stamp_;
        message.value = data_battery_;
        mutex_.unlock();
        return message;
    }

    void QuadrupedExampleNode::set_data_battery(std_msgs::msg::Float64 data)
    {
        mutex_.lock();
        // Data
        data_battery_ = data;
        data_battery_stamp_ = rclcpp::Node::now();
        // Message
        quadruped_example_skillset_interfaces::msg::DataBattery message;
        message.stamp = data_battery_stamp_;
        message.value = data_battery_;
        data_battery_pub_->publish(message);
        mutex_.unlock();
    }

    quadruped_example_skillset_interfaces::msg::DataBattery QuadrupedExampleNode::get_data_battery_hook()
    {
        quadruped_example_skillset_interfaces::msg::DataBattery message;
        message.stamp = data_battery_stamp_;
        message.value = data_battery_;
        return message;
    }

    void QuadrupedExampleNode::set_data_battery_hook(std_msgs::msg::Float64 data)
    {
        // Data
        data_battery_ = data;
        data_battery_stamp_ = rclcpp::Node::now();
        // Message
        quadruped_example_skillset_interfaces::msg::DataBattery message;
        message.stamp = data_battery_stamp_;
        message.value = data_battery_;
        data_battery_pub_->publish(message);
    }

    

    void QuadrupedExampleNode::data_battery_request_callback_(const quadruped_example_skillset_interfaces::msg::DataRequest::UniquePtr msg)
    {
        quadruped_example_skillset_interfaces::msg::DataBatteryResponse message;
        message.id = msg->id;
        mutex_.lock();
        if (data_battery_stamp_.nanoseconds() > 0) {
            message.has_data = true;
            message.stamp = data_battery_stamp_;
            message.value = data_battery_;
        }
        else {
            message.has_data = false;
            message.stamp = data_battery_stamp_;
        }
        mutex_.unlock();
        data_battery_response_pub_->publish(message);
    }

    
}
