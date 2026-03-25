import threading
import time
import rclpy
from rclpy.node import Node
from rclpy.executors import SingleThreadedExecutor, ExternalShutdownException

from std_msgs.msg import Empty
from quadruped_example_skillset_interfaces.msg import SkillsetStatus

from .resources import Resources
from .data import *
from .events import Events
from .skills import *
from .qos import *

class QuadrupedExampleSkillsetClient:
    def __init__(self, node_name: str, skillset_manager: str, data_subscription: bool = False, spin: bool = True):
        self.__node = Node(node_name=node_name, use_global_arguments=False)
        # SkillsetStatus
        self.__status_event = threading.Event()
        self.__status_subscriptions = self.__node.create_subscription(SkillsetStatus,
            f"{skillset_manager}/quadruped_example_skillset/status", 
            self.__status_callback,
            EVENT_QOS)
        self.__status_request = self.__node.create_publisher(Empty, 
            f"{skillset_manager}/quadruped_example_skillset/status_request",
            EVENT_QOS)
        self.__resources = Resources()
        # Data
        self.__data = Data(skillset_manager, data_subscription, self.__node)
        # Events
        self.__events = Events(skillset_manager, self.__node)
        # Skills
        self.__skills = Skills(skillset_manager, self.__node)

        ## Spinning
        self.__executor = SingleThreadedExecutor()
        self.__spin_thread = None
        if spin:
            self.__spin_thread = threading.Thread(target=self.__spin)
            self.__spin_thread.start()
            ## Init
            time.sleep(2)
            self.__get_status()

    ######## SkillsetStatus
    def __status_callback(self, msg: SkillsetStatus) -> None:
        self.__node.get_logger().debug(f"received skillset status {msg}")
        self.__resources.update_status(msg)
        
        self.skills.init_power.update_status(msg.skill_init_power)
        
        self.skills.safe_poweroff.update_status(msg.skill_safe_poweroff)
        
        self.skills.standup.update_status(msg.skill_standup)
        
        self.skills.sitdown.update_status(msg.skill_sitdown)
        
        self.skills.go_to.update_status(msg.skill_go_to)
        
        self.skills.recharge.update_status(msg.skill_recharge)
        
        self.skills.take_picture.update_status(msg.skill_take_picture)
        
        self.__status_event.set()

    def __get_status(self) -> None:
        self.__status_event.clear()
        self.__status_request.publish(Empty())
        self.__status_event.wait()

    def refresh_status(self) -> None:
        """Request fresh status from the skillset and update resources. Use before observing state."""
        self.__get_status()

    ######## Properties
    @property
    def node(self) -> Node:
        return self.__node

    @property
    def resources(self) -> Resources:
        return self.__resources

    @property
    def data(self) -> Data:
        return self.__data

    @property
    def events(self) -> Events:
        return self.__events

    @property
    def skills(self) -> Skills:
        return self.__skills

    ######## Spinning
    def __spin(self):
        try:
            rclpy.spin(self.__node, self.__executor)
        except (ExternalShutdownException, KeyboardInterrupt):
            pass
