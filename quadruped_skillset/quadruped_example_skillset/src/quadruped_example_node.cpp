#include "quadruped_example_skillset/Node.hpp"

#include <iostream>

using namespace quadruped_example_skillset;

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<QuadrupedExampleNode>("quadruped_example", "generated skillset"));
    rclcpp::shutdown();
    return 0;
}
