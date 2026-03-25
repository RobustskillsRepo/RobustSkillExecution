#!/usr/bin/env python3
"""
Launch the fault injection node (optional).
Run with the main stack for fault-injection testing.
"""

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node


def generate_launch_description():
    return LaunchDescription([
        DeclareLaunchArgument(
            "inject_on_skill",
            default_value="standup",
            description="Inject fault when this skill completes",
        ),
        DeclareLaunchArgument(
            "inject_event",
            default_value="set_poweroff",
            description="Event to inject: set_standing, set_sitting, set_poweroff, set_poweron",
        ),
        DeclareLaunchArgument(
            "use_parameters",
            default_value="false",
            description="Use ROS2 params instead of events (requires quadruped_impl_node_debug)",
        ),
        Node(
            package="fault_injection",
            executable="fault_injection_node.py",
            name="fault_injection_node",
            parameters=[
                {"inject_on_skill": LaunchConfiguration("inject_on_skill")},
                {"inject_event": LaunchConfiguration("inject_event")},
                {"use_parameters": LaunchConfiguration("use_parameters")},
            ],
        ),
    ])
