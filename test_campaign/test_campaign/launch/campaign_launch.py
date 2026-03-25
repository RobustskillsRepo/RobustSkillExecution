#!/usr/bin/env python3
"""
Launch the stack for test campaign runs.
Adds execution_trace_topic, skill_failure params, and optional fault_injection.
"""

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, GroupAction, IncludeLaunchDescription, TimerAction
from launch.conditions import IfCondition
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare


def generate_launch_description():
    return LaunchDescription([
        DeclareLaunchArgument("pddl_model_folder", default_value="0"),
        DeclareLaunchArgument("skill_failure_mode", default_value="none"),
        DeclareLaunchArgument("skill_failure_target", default_value=""),
        DeclareLaunchArgument("execution_trace_topic", default_value="execution_trace"),
        DeclareLaunchArgument("fault_injection_enabled", default_value="false"),
        DeclareLaunchArgument("inject_on_skill", default_value="standup"),
        DeclareLaunchArgument("inject_event", default_value="set_poweroff"),

        Node(
            package="quadruped_impl",
            executable="quadruped_impl_node",
            name="quadruped_example",
            parameters=[{
                "skill_failure_mode": LaunchConfiguration("skill_failure_mode"),
                "skill_failure_target": LaunchConfiguration("skill_failure_target"),
            }],
        ),
        Node(
            package="skill_execution_manager",
            executable="execution_manager_node.py",
            name="execution_manager_node",
            parameters=[{
                "execution_trace_topic": LaunchConfiguration("execution_trace_topic"),
            }],
        ),
        TimerAction(
            period=20.0,
            actions=[
                Node(
                    package="deliberative_layer",
                    executable="deliberative_node.py",
                    name="deliberative_node",
                    parameters=[{
                        "pddl_model_folder": LaunchConfiguration("pddl_model_folder"),
                        "execution_trace_topic": LaunchConfiguration("execution_trace_topic"),
                    }],
                ),
            ],
        ),
        GroupAction(
            condition=IfCondition(LaunchConfiguration("fault_injection_enabled")),
            actions=[
                Node(
                    package="fault_injection",
                    executable="fault_injection_node.py",
                    name="fault_injection_node",
                    parameters=[{
                        "inject_on_skill": LaunchConfiguration("inject_on_skill"),
                        "inject_event": LaunchConfiguration("inject_event"),
                    }],
                ),
            ],
        ),
    ])
