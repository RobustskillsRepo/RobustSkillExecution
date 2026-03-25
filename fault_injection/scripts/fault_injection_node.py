#!/usr/bin/env python3
"""
Fault injection node: optionally triggers events (resource changes) to disrupt mission execution.
Uses:
  1. EventRequest topic - publishes set_standing, set_sitting, set_poweroff, set_poweron (guards apply).
  2. ROS2 parameters - sets resource_motor_state / resource_spot_state (quadruped_impl_node_debug only).
"""

import uuid
import threading
from typing import Callable, Dict, List, Optional

import rclpy
from rclpy.executors import ExternalShutdownException
from rclpy.node import Node
from rclpy.qos import QoSProfile, ReliabilityPolicy, DurabilityPolicy, HistoryPolicy, LivelinessPolicy

from rcl_interfaces.srv import SetParameters
from quadruped_example_skillset_interfaces.msg import (
    EventRequest,
    SkillInitPowerResponse,
    SkillSafePoweroffResponse,
    SkillStandupResponse,
    SkillSitdownResponse,
    SkillGoToResponse,
    SkillRechargeResponse,
    SkillTakePictureResponse,
)

# Match quadruped_example_skillset_client EVENT_QOS for compatibility
EVENT_QOS = QoSProfile(
    reliability=ReliabilityPolicy.RELIABLE,
    durability=DurabilityPolicy.VOLATILE,
    history=HistoryPolicy.KEEP_ALL,
    liveliness=LivelinessPolicy.AUTOMATIC,
)


# Skill response types for on_skill_response trigger
SKILL_RESPONSE_TYPES = {
    "init_power": SkillInitPowerResponse,
    "safe_poweroff": SkillSafePoweroffResponse,
    "standup": SkillStandupResponse,
    "sitdown": SkillSitdownResponse,
    "go_to": SkillGoToResponse,
    "recharge": SkillRechargeResponse,
    "take_picture": SkillTakePictureResponse,
}

SUCCESS_RESULT = 0

class FaultInjectionNode(Node):
    """ROS2 node that injects faults via events or parameters."""

    EVENT_NAMES = ["set_standing", "set_sitting", "set_poweroff", "set_poweron"]
    RESOURCE_PARAMS = ["resource_motor_state", "resource_spot_state"]
    VALID_MOTOR = ["On", "Off"]
    VALID_SPOT = ["Sitting", "Standing"]

    def __init__(self):
        super().__init__("fault_injection_node")
        self.declare_parameter("skillset_manager", "quadruped_example")
        self.declare_parameter("target_node", "quadruped_example")
        self.declare_parameter("trigger_mode", "on_skill_response")
        self.declare_parameter("inject_on_skill", "standup")
        self.declare_parameter("inject_event", "set_poweroff")
        self.declare_parameter("inject_param_motor_state", "Off")
        self.declare_parameter("inject_param_spot_state", "Sitting")
        self.declare_parameter("inject_delay_sec", 0.0)
        self.declare_parameter("use_parameters", False)
        self.declare_parameter("randomize", False)

        self._skillset = self.get_parameter("skillset_manager").value
        self._target_node = self.get_parameter("target_node").value
        self._trigger_mode = self.get_parameter("trigger_mode").value
        self._inject_on_skill = self.get_parameter("inject_on_skill").value
        self._inject_event = self.get_parameter("inject_event").value
        self._use_params = self.get_parameter("use_parameters").value
        self._inject_delay = self.get_parameter("inject_delay_sec").value
        self._randomize = self.get_parameter("randomize").value

        self._inject_count = 0
        self._lock = threading.Lock()

        event_topic = f"{self._skillset}/quadruped_example_skillset/event_request"
        self._event_pub = self.create_publisher(EventRequest, event_topic, EVENT_QOS)

        # Subscribe to skill response topic (more reliable than SkillsetStatus)
        if self._trigger_mode == "on_skill_response" and self._inject_on_skill in SKILL_RESPONSE_TYPES:
            resp_type = SKILL_RESPONSE_TYPES[self._inject_on_skill]
            resp_topic = f"{self._skillset}/quadruped_example_skillset/skill/{self._inject_on_skill}/response"
            self.create_subscription(resp_type, resp_topic, self._on_skill_response, EVENT_QOS)

        param_svc = f"{self._target_node}/set_parameters"
        self._client = self.create_client(SetParameters, param_svc)
        if self._use_params:
            self.get_logger().info("Waiting for parameter service (quadruped_impl_node_debug)...")
            while not self._client.wait_for_service(timeout_sec=1.0):
                self.get_logger().info("Waiting for parameter service...")

        self.get_logger().info(
            f"Fault injection ready: trigger={self._trigger_mode}, inject_on={self._inject_on_skill}, "
            f"event={self._inject_event}, use_params={self._use_params}"
        )
        if self._trigger_mode == "on_skill_response":
            self.get_logger().info(
                f"Subscribed to skill response: {self._skillset}/quadruped_example_skillset/skill/{self._inject_on_skill}/response"
            )

    def _on_skill_response(self, msg) -> None:
        """Trigger fault injection when configured skill completes successfully (once only)."""
        if getattr(msg, "result", -1) != SUCCESS_RESULT:
            return
        with self._lock:
            if self._inject_count > 0:
                return  # Already injected once
            self._inject_count = 1  # Reserve now to prevent re-entry
        self.get_logger().info(f"Skill '{self._inject_on_skill}' completed (SUCCESS), injecting fault (once)")
        self._schedule_inject()

    def _schedule_inject(self) -> None:
        if self._inject_delay > 0:
            threading.Timer(self._inject_delay, self._do_inject).start()
        else:
            self._do_inject()

    def _do_inject(self) -> None:
        if self._use_params:
            self._inject_via_parameters()
        else:
            self._inject_via_event()

    def _inject_via_event(self) -> None:
        event = self._inject_event
        if self._randomize:
            import random
            event = random.choice(self.EVENT_NAMES)
        uid = str(uuid.uuid4())[:8]
        self.get_logger().info(f"Injecting event: {event} (id={uid})")
        self._event_pub.publish(EventRequest(id=uid, name=event))

    def _inject_via_parameters(self) -> None:
        if not self._client.service_is_ready():
            self.get_logger().warn("Parameter service not available (run quadruped_impl_node_debug)")
            return
        from rcl_interfaces.srv import SetParameters
        from rcl_interfaces.msg import Parameter as ParamMsg, ParameterValue, ParameterType

        motor = self.get_parameter("inject_param_motor_state").value
        spot = self.get_parameter("inject_param_spot_state").value
        if self._randomize:
            import random
            motor = random.choice(self.VALID_MOTOR)
            spot = random.choice(self.VALID_SPOT)

        pv_motor = ParameterValue(type=ParameterType.PARAMETER_STRING, string_value=motor)
        pv_spot = ParameterValue(type=ParameterType.PARAMETER_STRING, string_value=spot)
        params = [
            ParamMsg(name="resource_motor_state", value=pv_motor),
            ParamMsg(name="resource_spot_state", value=pv_spot),
        ]
        req = SetParameters.Request()
        req.parameters = params
        fut = self._client.call_async(req)
        fut.add_done_callback(lambda f: self._on_param_result(f, motor, spot))

    def _on_param_result(self, future, motor: str, spot: str) -> None:
        try:
            result = future.result()
            if all(r.successful for r in result.results):
                self.get_logger().info(f"Injected params: motor_state={motor}, spot_state={spot}")
            else:
                self.get_logger().warn("Parameter injection failed (is quadruped_impl_node_debug running?)")
        except Exception as e:
            self.get_logger().error(f"Parameter injection error: {e}")


def main(args=None):
    rclpy.init(args=args)
    node = FaultInjectionNode()
    try:
        rclpy.spin(node)
    except (KeyboardInterrupt, ExternalShutdownException):
        pass
    finally:
        node.destroy_node()
        rclpy.try_shutdown()


if __name__ == "__main__":
    main()
