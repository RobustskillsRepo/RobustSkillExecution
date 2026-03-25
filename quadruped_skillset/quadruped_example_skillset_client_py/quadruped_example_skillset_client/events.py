from collections import defaultdict
from functools import partial
import rclpy.node
import threading
from typing import Dict, Callable

from quadruped_example_skillset_interfaces.msg import EventRequest, EventResponse

from .uid import new_id
from .qos import *


class Events:
    def __init__(self, skillset_manager: str, node: rclpy.node.Node) -> None:
        self.__node = node
        self.__events = ["set_standing", "set_sitting", "set_poweroff", "set_poweron"]
        self.__thread_events : Dict[str, threading.Event] = defaultdict(threading.Event)
        self.__responses : Dict[str, EventResponse] = dict()
        self.__event_response = node.create_subscription(EventResponse,
                f"{skillset_manager}/quadruped_example_skillset/event_response", 
                self.__event_callback,
                EVENT_QOS)
        self.__event_request = node.create_publisher(EventRequest, 
            f"{skillset_manager}/quadruped_example_skillset/event_request",
            EVENT_QOS)

    def __event_callback(self, msg: EventResponse):
        self.__responses[msg.id] = msg
        self.__thread_events[msg.id].set()

    def __event(self, name: str) -> int:
        uid = new_id()
        self.__node.get_logger().debug(f"event request {uid} {name}")
        self.__thread_events[uid].clear()
        self.__event_request.publish(EventRequest(id=uid, name=name))
        self.__node.get_logger().debug(f"waiting event response {uid}")
        self.__thread_events[uid].wait()
        response = self.__responses[uid].response
        self.__node.get_logger().debug(f"event response {uid} {response}")
        return response

    
    @property
    def set_standing(self) -> Callable[[], int]:
        return partial(self.__event, 'set_standing')
    
    
    @property
    def set_sitting(self) -> Callable[[], int]:
        return partial(self.__event, 'set_sitting')
    
    
    @property
    def set_poweroff(self) -> Callable[[], int]:
        return partial(self.__event, 'set_poweroff')
    
    
    @property
    def set_poweron(self) -> Callable[[], int]:
        return partial(self.__event, 'set_poweron')
    
    

    def __getitem__(self, item):
        return getattr(self, item)

    def __iter__(self):
        return iter(self.__events)

    def __len__(self):
        return len(self.__events)

    def __nonzero__(self):
        return len(self.__events) > 0
