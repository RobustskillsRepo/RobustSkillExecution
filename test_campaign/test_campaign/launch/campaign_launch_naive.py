#!/usr/bin/env python3
"""
Launch the stack for test campaign runs with NAIVE executor (quadruped_mission).
Uses quadruped_mission instead of skill_execution_manager + deliberative_layer.
Same params: execution_trace_topic, pddl_model_folder, skill_failure, fault_injection.
"""

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, GroupAction, TimerAction
from launch.conditions import IfCondition
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node


def generate_launch_description():
    return LaunchDescription([
        DeclareLaunchArgument("pddl_model_folder", default_value="0"),
        DeclareLaunchArgument("skill_failure_mode", default_value="none"),
        DeclareLaunchArgument("skill_failure_target", default_value=""),
        DeclareLaunchArgument("execution_trace_topic", default_value="execution_trace"),
        DeclareLaunchArgument("fault_injection_enabled", default_value="false"),
        DeclareLaunchArgument("inject_on_skill", default_value="standup"),
        DeclareLaunchArgument("inject_event", default_value="set_poweroff"),
        DeclareLaunchArgument("post_skill_delay_sec", default_value="0.0"),
        DeclareLaunchArgument("inject_delay_sec", default_value="0.0"),

        Node(
            package="quadruped_impl",
            executable="quadruped_impl_node",
            name="quadruped_example",
            parameters=[{
                "skill_failure_mode": LaunchConfiguration("skill_failure_mode"),
                "skill_failure_target": LaunchConfiguration("skill_failure_target"),
            }],
        ),
        TimerAction(
            period=8.0,
            actions=[
                Node(
                    package="quadruped_example_skillset_client",
                    executable="quadruped_mission.py",
                    name="quadruped_mission",
                    additional_env={"PYTHONUNBUFFERED": "1"},
                    parameters=[{
                        "execution_trace_topic": LaunchConfiguration("execution_trace_topic"),
                        "pddl_model_folder": LaunchConfiguration("pddl_model_folder"),
                        "post_skill_delay_sec": LaunchConfiguration("post_skill_delay_sec"),
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
                    additional_env={"PYTHONUNBUFFERED": "1"},
                    parameters=[{
                        "inject_on_skill": LaunchConfiguration("inject_on_skill"),
                        "inject_event": LaunchConfiguration("inject_event"),
                        "inject_delay_sec": LaunchConfiguration("inject_delay_sec"),
                    }],
                ),
            ],
        ),
    ])
