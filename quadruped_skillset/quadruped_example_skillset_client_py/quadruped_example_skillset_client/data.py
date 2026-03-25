from .qos import HARD_STATE_QOS
import rclpy.node
import threading
from typing import Optional

from builtin_interfaces.msg import Time
from quadruped_example_skillset_interfaces.msg import DataRequest

from .uid import new_id
from .qos import *

from quadruped_example_skillset_interfaces.msg import DataLocation, DataLocationResponse
from quadruped_example_skillset_interfaces.msg import DataBattery, DataBatteryResponse


class LocationData:
    def __init__(self, skillset_manager: str, subscription: bool, node: rclpy.node.Node) -> None:
        self.__event = threading.Event()
        self.__data : Optional[DataLocation] = None
        self.__id : str = ""
        self.__request = node.create_publisher(DataRequest, 
            f"{skillset_manager}/quadruped_example_skillset/data/location/request",
            EVENT_QOS)

        if subscription:
            self.__sub = node.create_subscription(DataLocation,
                f"{skillset_manager}/quadruped_example_skillset/data/location", 
                self.__data_callback,
                HARD_STATE_QOS)
        else:
            self.__sub = node.create_subscription(DataLocationResponse,
                f"{skillset_manager}/quadruped_example_skillset/data/location/response", 
                self.__response_callback,
                EVENT_QOS)
        self.__subscription_flag = subscription

    def __response_callback(self, msg: DataLocationResponse):
        if msg.has_data:
            data = DataLocation()
            data.stamp = msg.stamp
            data.value = msg.value
            self.__data = data
        else:
            self.__data = None
        self.__event.set()

    def __data_callback(self, msg: DataLocation):
            self.__data = msg

    @property
    def data(self) -> Optional[DataLocation]:
        return self.__data

    def get(self) -> Optional[DataLocation]:
        if self.__subscription_flag:
            return self.data
        else:
            self.__id = new_id()
            self.__event.clear()
            self.__request.publish(DataRequest(id=self.__id))
            self.__event.wait()
            return self.data

class BatteryData:
    def __init__(self, skillset_manager: str, subscription: bool, node: rclpy.node.Node) -> None:
        self.__event = threading.Event()
        self.__data : Optional[DataBattery] = None
        self.__id : str = ""
        self.__request = node.create_publisher(DataRequest, 
            f"{skillset_manager}/quadruped_example_skillset/data/battery/request",
            EVENT_QOS)

        if subscription:
            self.__sub = node.create_subscription(DataBattery,
                f"{skillset_manager}/quadruped_example_skillset/data/battery", 
                self.__data_callback,
                HARD_STATE_QOS)
        else:
            self.__sub = node.create_subscription(DataBatteryResponse,
                f"{skillset_manager}/quadruped_example_skillset/data/battery/response", 
                self.__response_callback,
                EVENT_QOS)
        self.__subscription_flag = subscription

    def __response_callback(self, msg: DataBatteryResponse):
        if msg.has_data:
            data = DataBattery()
            data.stamp = msg.stamp
            data.value = msg.value
            self.__data = data
        else:
            self.__data = None
        self.__event.set()

    def __data_callback(self, msg: DataBattery):
            self.__data = msg

    @property
    def data(self) -> Optional[DataBattery]:
        return self.__data

    def get(self) -> Optional[DataBattery]:
        if self.__subscription_flag:
            return self.data
        else:
            self.__id = new_id()
            self.__event.clear()
            self.__request.publish(DataRequest(id=self.__id))
            self.__event.wait()
            return self.data


class Data:
    def __init__(self, skillset_manager: str, subscription: bool, node: rclpy.node.Node) -> None:
        
        self.__location = LocationData(skillset_manager, subscription, node)
        
        self.__battery = BatteryData(skillset_manager, subscription, node)
        self.__data = ['location', 'battery']

    
    @property
    def location(self) -> LocationData:
        return self.__location
    
    @property
    def battery(self) -> BatteryData:
        return self.__battery
    

    def __getitem__(self, item):
        return getattr(self, item)

    def __iter__(self):
        return iter(self.__data)

    def __len__(self):
        return len(self.__data)

    def __nonzero__(self):
        return len(self.__data) > 0
