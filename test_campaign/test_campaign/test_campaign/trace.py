"""Trace event collection from execution_trace topic."""

import json
import threading
from typing import Any, Callable, Dict, List, Optional

import rclpy
from rclpy.node import Node
from test_campaign_msgs.msg import ExecutionTrace


class TraceCollector(Node):
    """Subscribes to execution_trace topic and builds RunRecord from events."""

    def __init__(self, topic: str = "execution_trace"):
        super().__init__("trace_collector")
        self._topic = topic
        self._events: List[Dict[str, Any]] = []
        self._lock = threading.Lock()
        self._plan_complete_event = threading.Event()
        self._plan_start_event = threading.Event()
        self._sub = self.create_subscription(
            ExecutionTrace,
            topic,
            self._on_trace,
            10,
        )

    def _on_trace(self, msg: ExecutionTrace) -> None:
        try:
            data = json.loads(msg.data) if msg.data else {}
        except json.JSONDecodeError:
            data = {}
        event = {"event": msg.event, "data": data}
        with self._lock:
            self._events.append(event)
        if msg.event == "plan_start":
            self._plan_start_event.set()
        if msg.event == "plan_complete":
            self._plan_complete_event.set()

    def wait_plan_start(self, timeout_sec: float = 30.0) -> bool:
        """Wait for plan_start event. Returns True if received. Use to detect startup hangs."""
        return self._plan_start_event.wait(timeout=timeout_sec)

    def wait_plan_complete(self, timeout_sec: float = 300.0) -> bool:
        """Wait for plan_complete event. Returns True if received."""
        return self._plan_complete_event.wait(timeout=timeout_sec)

    def get_events(self) -> List[Dict[str, Any]]:
        """Return copy of collected events."""
        with self._lock:
            return list(self._events)

    def reset(self) -> None:
        """Clear events and reset plan_complete flag."""
        with self._lock:
            self._events.clear()
        self._plan_complete_event.clear()
        self._plan_start_event.clear()
