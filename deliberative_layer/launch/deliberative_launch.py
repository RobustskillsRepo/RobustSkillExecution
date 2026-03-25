#!/usr/bin/env python3
"""
Launch the three-layer architecture and run the experiment:
  1. quadruped_impl (functional layer - skillset)
  2. skill_execution_manager (executive layer)
  3. deliberative_layer (deliberative layer - FF planning)
"""

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, GroupAction, TimerAction
from launch.conditions import IfCondition
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node


def generate_launch_description():
    return LaunchDescription([
        DeclareLaunchArgument(
            "domain_path",
            default_value="",
            description="PDDL domain path (empty = use package share default)",
        ),
        DeclareLaunchArgument(
            "problem_path",
            default_value="",
            description="PDDL problem path (empty = use package share default)",
        ),
        DeclareLaunchArgument(
            "pddl_model_folder",
            default_value="0",
            description="PDDL model folder number (e.g. 0, 1, 2) for fault injection variants",
        ),
        DeclareLaunchArgument(
            "ff_binary",
            default_value="ff",
            description="FF planner binary name",
        ),
        DeclareLaunchArgument(
            "max_retries",
            default_value="3",
            description="Max re-planning cycles on failure",
        ),
        DeclareLaunchArgument(
            "skill_failure_mode",
            default_value="none",
            description="Skill failure mode for testing: none, once, or always",
        ),
        DeclareLaunchArgument(
            "skill_failure_target",
            default_value="",
            description="Skill name to fail (e.g. standup, go_to) when skill_failure_mode is set",
        ),
        DeclareLaunchArgument(
            "fault_injection_enabled",
            default_value="false",
            description="Enable fault injection node for test campaigns",
        ),
        DeclareLaunchArgument(
            "inject_on_skill",
            default_value="standup",
            description="Inject fault when this skill completes (if fault_injection_enabled)",
        ),
        DeclareLaunchArgument(
            "inject_event",
            default_value="set_poweroff",
            description="Event to inject (if fault_injection_enabled)",
        ),
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
        TimerAction(
            period=15.0,
            actions=[
                Node(
                    package="deliberative_layer",
                    executable="deliberative_node.py",
                    name="deliberative_node",
                    parameters=[{
                        "domain_path": LaunchConfiguration("domain_path"),
                        "problem_path": LaunchConfiguration("problem_path"),
                        "pddl_model_folder": LaunchConfiguration("pddl_model_folder"),
                        "ff_binary": LaunchConfiguration("ff_binary"),
                        "max_retries": LaunchConfiguration("max_retries"),
                    }],
                ),
            ],
        ),
    ])
