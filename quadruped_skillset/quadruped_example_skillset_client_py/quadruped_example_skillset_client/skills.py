from collections import defaultdict

from inflection import camelize
import rclpy.node
import threading
from typing import Dict
from .qos import *

from quadruped_example_skillset_interfaces.msg import SkillInterrupt, SkillsetStatus
from .uid import new_id

from quadruped_example_skillset_interfaces.msg import SkillInitPowerRequest, SkillInitPowerResponse, SkillInitPowerStatus

class InitPowerSkill:
    def __init__(self, skillset_manager: str, node: rclpy.node.Node) -> None:
        self.__node = node
        self.__response_sub = node.create_subscription(SkillInitPowerResponse,
            f"{skillset_manager}/quadruped_example_skillset/skill/init_power/response", 
            self.__response_cb,
            EVENT_QOS)
        
        self.__request_pub = node.create_publisher(SkillInitPowerRequest,
            f"{skillset_manager}/quadruped_example_skillset/skill/init_power/request",
            1)
        self.__interrupt_pub = node.create_publisher(SkillInterrupt, 
            f"{skillset_manager}/quadruped_example_skillset/skill/init_power/interrupt",
            EVENT_QOS)
        self.__progress_msgs : Dict[str, SkillInitPowerProgress] = dict()
        self.__response_msgs : Dict[str, SkillInitPowerResponse] = dict()
        self.__events : Dict[str, threading.Event] = defaultdict(threading.Event)

        self.__status: SkillInitPowerStatus = None

    def start(self) -> str:
        request = SkillInitPowerRequest()
        request.id = new_id()
        self.__node.get_logger().debug(f"starting skill 'init_power' with id {request.id}")
        self.__events[request.id].clear()
        self.__request_pub.publish(request)
        return request.id

    def interrupt(self, id: str):
        self.__node.get_logger().debug(f"interrupting skill 'init_power' with id {id}")
        self.__interrupt_pub.publish(SkillInterrupt(id=id))

    def wait_result(self, id: str) -> SkillInitPowerResponse:
        self.__node.get_logger().debug(f"waiting skill 'init_power' {id}")
        self.__events[id].wait()
        msg = self.__response_msgs[id]
        self.__node.get_logger().debug(f"got skill 'init_power' response {msg}")
        return msg

    def __response_cb(self, msg: SkillInitPowerResponse):
        self.__node.get_logger().debug(f"received '' response {msg}")
        self.__response_msgs[msg.id] = msg
        self.__events[msg.id].set()

    
    def update_status(self, status: SkillInitPowerStatus):
        self.__status = status

    def status(self) -> SkillInitPowerStatus:
        return self.__status

    def is_ready(self) -> bool:
        return self.__status.state == SkillInitPowerStatus.READY

    def is_running(self) -> bool:
        return self.__status.state == SkillInitPowerStatus.RUNNING

    def is_interrupting(self) -> bool:
        return self.__status.state == SkillInitPowerStatus.INTERRUPTING



from quadruped_example_skillset_interfaces.msg import SkillSafePoweroffRequest, SkillSafePoweroffResponse, SkillSafePoweroffStatus

class SafePoweroffSkill:
    def __init__(self, skillset_manager: str, node: rclpy.node.Node) -> None:
        self.__node = node
        self.__response_sub = node.create_subscription(SkillSafePoweroffResponse,
            f"{skillset_manager}/quadruped_example_skillset/skill/safe_poweroff/response", 
            self.__response_cb,
            EVENT_QOS)
        
        self.__request_pub = node.create_publisher(SkillSafePoweroffRequest,
            f"{skillset_manager}/quadruped_example_skillset/skill/safe_poweroff/request",
            1)
        self.__interrupt_pub = node.create_publisher(SkillInterrupt, 
            f"{skillset_manager}/quadruped_example_skillset/skill/safe_poweroff/interrupt",
            EVENT_QOS)
        self.__progress_msgs : Dict[str, SkillSafePoweroffProgress] = dict()
        self.__response_msgs : Dict[str, SkillSafePoweroffResponse] = dict()
        self.__events : Dict[str, threading.Event] = defaultdict(threading.Event)

        self.__status: SkillSafePoweroffStatus = None

    def start(self) -> str:
        request = SkillSafePoweroffRequest()
        request.id = new_id()
        self.__node.get_logger().debug(f"starting skill 'safe_poweroff' with id {request.id}")
        self.__events[request.id].clear()
        self.__request_pub.publish(request)
        return request.id

    def interrupt(self, id: str):
        self.__node.get_logger().debug(f"interrupting skill 'safe_poweroff' with id {id}")
        self.__interrupt_pub.publish(SkillInterrupt(id=id))

    def wait_result(self, id: str) -> SkillSafePoweroffResponse:
        self.__node.get_logger().debug(f"waiting skill 'safe_poweroff' {id}")
        self.__events[id].wait()
        msg = self.__response_msgs[id]
        self.__node.get_logger().debug(f"got skill 'safe_poweroff' response {msg}")
        return msg

    def __response_cb(self, msg: SkillSafePoweroffResponse):
        self.__node.get_logger().debug(f"received '' response {msg}")
        self.__response_msgs[msg.id] = msg
        self.__events[msg.id].set()

    
    def update_status(self, status: SkillSafePoweroffStatus):
        self.__status = status

    def status(self) -> SkillSafePoweroffStatus:
        return self.__status

    def is_ready(self) -> bool:
        return self.__status.state == SkillSafePoweroffStatus.READY

    def is_running(self) -> bool:
        return self.__status.state == SkillSafePoweroffStatus.RUNNING

    def is_interrupting(self) -> bool:
        return self.__status.state == SkillSafePoweroffStatus.INTERRUPTING



from quadruped_example_skillset_interfaces.msg import SkillStandupRequest, SkillStandupResponse, SkillStandupStatus

class StandupSkill:
    def __init__(self, skillset_manager: str, node: rclpy.node.Node) -> None:
        self.__node = node
        self.__response_sub = node.create_subscription(SkillStandupResponse,
            f"{skillset_manager}/quadruped_example_skillset/skill/standup/response", 
            self.__response_cb,
            EVENT_QOS)
        
        self.__request_pub = node.create_publisher(SkillStandupRequest,
            f"{skillset_manager}/quadruped_example_skillset/skill/standup/request",
            1)
        self.__interrupt_pub = node.create_publisher(SkillInterrupt, 
            f"{skillset_manager}/quadruped_example_skillset/skill/standup/interrupt",
            EVENT_QOS)
        self.__progress_msgs : Dict[str, SkillStandupProgress] = dict()
        self.__response_msgs : Dict[str, SkillStandupResponse] = dict()
        self.__events : Dict[str, threading.Event] = defaultdict(threading.Event)

        self.__status: SkillStandupStatus = None

    def start(self) -> str:
        request = SkillStandupRequest()
        request.id = new_id()
        self.__node.get_logger().debug(f"starting skill 'standup' with id {request.id}")
        self.__events[request.id].clear()
        self.__request_pub.publish(request)
        return request.id

    def interrupt(self, id: str):
        self.__node.get_logger().debug(f"interrupting skill 'standup' with id {id}")
        self.__interrupt_pub.publish(SkillInterrupt(id=id))

    def wait_result(self, id: str) -> SkillStandupResponse:
        self.__node.get_logger().debug(f"waiting skill 'standup' {id}")
        self.__events[id].wait()
        msg = self.__response_msgs[id]
        self.__node.get_logger().debug(f"got skill 'standup' response {msg}")
        return msg

    def __response_cb(self, msg: SkillStandupResponse):
        self.__node.get_logger().debug(f"received '' response {msg}")
        self.__response_msgs[msg.id] = msg
        self.__events[msg.id].set()

    
    def update_status(self, status: SkillStandupStatus):
        self.__status = status

    def status(self) -> SkillStandupStatus:
        return self.__status

    def is_ready(self) -> bool:
        return self.__status.state == SkillStandupStatus.READY

    def is_running(self) -> bool:
        return self.__status.state == SkillStandupStatus.RUNNING

    def is_interrupting(self) -> bool:
        return self.__status.state == SkillStandupStatus.INTERRUPTING



from quadruped_example_skillset_interfaces.msg import SkillSitdownRequest, SkillSitdownResponse, SkillSitdownStatus

class SitdownSkill:
    def __init__(self, skillset_manager: str, node: rclpy.node.Node) -> None:
        self.__node = node
        self.__response_sub = node.create_subscription(SkillSitdownResponse,
            f"{skillset_manager}/quadruped_example_skillset/skill/sitdown/response", 
            self.__response_cb,
            EVENT_QOS)
        
        self.__request_pub = node.create_publisher(SkillSitdownRequest,
            f"{skillset_manager}/quadruped_example_skillset/skill/sitdown/request",
            1)
        self.__interrupt_pub = node.create_publisher(SkillInterrupt, 
            f"{skillset_manager}/quadruped_example_skillset/skill/sitdown/interrupt",
            EVENT_QOS)
        self.__progress_msgs : Dict[str, SkillSitdownProgress] = dict()
        self.__response_msgs : Dict[str, SkillSitdownResponse] = dict()
        self.__events : Dict[str, threading.Event] = defaultdict(threading.Event)

        self.__status: SkillSitdownStatus = None

    def start(self) -> str:
        request = SkillSitdownRequest()
        request.id = new_id()
        self.__node.get_logger().debug(f"starting skill 'sitdown' with id {request.id}")
        self.__events[request.id].clear()
        self.__request_pub.publish(request)
        return request.id

    def interrupt(self, id: str):
        self.__node.get_logger().debug(f"interrupting skill 'sitdown' with id {id}")
        self.__interrupt_pub.publish(SkillInterrupt(id=id))

    def wait_result(self, id: str) -> SkillSitdownResponse:
        self.__node.get_logger().debug(f"waiting skill 'sitdown' {id}")
        self.__events[id].wait()
        msg = self.__response_msgs[id]
        self.__node.get_logger().debug(f"got skill 'sitdown' response {msg}")
        return msg

    def __response_cb(self, msg: SkillSitdownResponse):
        self.__node.get_logger().debug(f"received '' response {msg}")
        self.__response_msgs[msg.id] = msg
        self.__events[msg.id].set()

    
    def update_status(self, status: SkillSitdownStatus):
        self.__status = status

    def status(self) -> SkillSitdownStatus:
        return self.__status

    def is_ready(self) -> bool:
        return self.__status.state == SkillSitdownStatus.READY

    def is_running(self) -> bool:
        return self.__status.state == SkillSitdownStatus.RUNNING

    def is_interrupting(self) -> bool:
        return self.__status.state == SkillSitdownStatus.INTERRUPTING



from quadruped_example_skillset_interfaces.msg import SkillGoToRequest, SkillGoToResponse, SkillGoToStatus
from quadruped_example_skillset_interfaces.msg import SkillGoToInput

class GoToSkill:
    def __init__(self, skillset_manager: str, node: rclpy.node.Node) -> None:
        self.__node = node
        self.__response_sub = node.create_subscription(SkillGoToResponse,
            f"{skillset_manager}/quadruped_example_skillset/skill/go_to/response", 
            self.__response_cb,
            EVENT_QOS)
        
        self.__request_pub = node.create_publisher(SkillGoToRequest,
            f"{skillset_manager}/quadruped_example_skillset/skill/go_to/request",
            1)
        self.__interrupt_pub = node.create_publisher(SkillInterrupt, 
            f"{skillset_manager}/quadruped_example_skillset/skill/go_to/interrupt",
            EVENT_QOS)
        self.__progress_msgs : Dict[str, SkillGoToProgress] = dict()
        self.__response_msgs : Dict[str, SkillGoToResponse] = dict()
        self.__events : Dict[str, threading.Event] = defaultdict(threading.Event)

        self.__status: SkillGoToStatus = None

    def start(self, input: SkillGoToInput) -> str:
        request = SkillGoToRequest()
        request.id = new_id()
        request.input = input
        self.__node.get_logger().debug(f"starting skill 'go_to' with id {request.id}")
        self.__events[request.id].clear()
        self.__request_pub.publish(request)
        return request.id

    def interrupt(self, id: str):
        self.__node.get_logger().debug(f"interrupting skill 'go_to' with id {id}")
        self.__interrupt_pub.publish(SkillInterrupt(id=id))

    def wait_result(self, id: str) -> SkillGoToResponse:
        self.__node.get_logger().debug(f"waiting skill 'go_to' {id}")
        self.__events[id].wait()
        msg = self.__response_msgs[id]
        self.__node.get_logger().debug(f"got skill 'go_to' response {msg}")
        return msg

    def __response_cb(self, msg: SkillGoToResponse):
        self.__node.get_logger().debug(f"received '' response {msg}")
        self.__response_msgs[msg.id] = msg
        self.__events[msg.id].set()

    
    def update_status(self, status: SkillGoToStatus):
        self.__status = status

    def status(self) -> SkillGoToStatus:
        return self.__status

    def is_ready(self) -> bool:
        return self.__status.state == SkillGoToStatus.READY

    def is_running(self) -> bool:
        return self.__status.state == SkillGoToStatus.RUNNING

    def is_interrupting(self) -> bool:
        return self.__status.state == SkillGoToStatus.INTERRUPTING



from quadruped_example_skillset_interfaces.msg import SkillRechargeRequest, SkillRechargeResponse, SkillRechargeStatus

class RechargeSkill:
    def __init__(self, skillset_manager: str, node: rclpy.node.Node) -> None:
        self.__node = node
        self.__response_sub = node.create_subscription(SkillRechargeResponse,
            f"{skillset_manager}/quadruped_example_skillset/skill/recharge/response", 
            self.__response_cb,
            EVENT_QOS)
        
        self.__request_pub = node.create_publisher(SkillRechargeRequest,
            f"{skillset_manager}/quadruped_example_skillset/skill/recharge/request",
            1)
        self.__interrupt_pub = node.create_publisher(SkillInterrupt, 
            f"{skillset_manager}/quadruped_example_skillset/skill/recharge/interrupt",
            EVENT_QOS)
        self.__progress_msgs : Dict[str, SkillRechargeProgress] = dict()
        self.__response_msgs : Dict[str, SkillRechargeResponse] = dict()
        self.__events : Dict[str, threading.Event] = defaultdict(threading.Event)

        self.__status: SkillRechargeStatus = None

    def start(self) -> str:
        request = SkillRechargeRequest()
        request.id = new_id()
        self.__node.get_logger().debug(f"starting skill 'recharge' with id {request.id}")
        self.__events[request.id].clear()
        self.__request_pub.publish(request)
        return request.id

    def interrupt(self, id: str):
        self.__node.get_logger().debug(f"interrupting skill 'recharge' with id {id}")
        self.__interrupt_pub.publish(SkillInterrupt(id=id))

    def wait_result(self, id: str) -> SkillRechargeResponse:
        self.__node.get_logger().debug(f"waiting skill 'recharge' {id}")
        self.__events[id].wait()
        msg = self.__response_msgs[id]
        self.__node.get_logger().debug(f"got skill 'recharge' response {msg}")
        return msg

    def __response_cb(self, msg: SkillRechargeResponse):
        self.__node.get_logger().debug(f"received '' response {msg}")
        self.__response_msgs[msg.id] = msg
        self.__events[msg.id].set()

    
    def update_status(self, status: SkillRechargeStatus):
        self.__status = status

    def status(self) -> SkillRechargeStatus:
        return self.__status

    def is_ready(self) -> bool:
        return self.__status.state == SkillRechargeStatus.READY

    def is_running(self) -> bool:
        return self.__status.state == SkillRechargeStatus.RUNNING

    def is_interrupting(self) -> bool:
        return self.__status.state == SkillRechargeStatus.INTERRUPTING



from quadruped_example_skillset_interfaces.msg import SkillTakePictureRequest, SkillTakePictureResponse, SkillTakePictureStatus

class TakePictureSkill:
    def __init__(self, skillset_manager: str, node: rclpy.node.Node) -> None:
        self.__node = node
        self.__response_sub = node.create_subscription(SkillTakePictureResponse,
            f"{skillset_manager}/quadruped_example_skillset/skill/take_picture/response", 
            self.__response_cb,
            EVENT_QOS)
        
        self.__request_pub = node.create_publisher(SkillTakePictureRequest,
            f"{skillset_manager}/quadruped_example_skillset/skill/take_picture/request",
            1)
        self.__interrupt_pub = node.create_publisher(SkillInterrupt, 
            f"{skillset_manager}/quadruped_example_skillset/skill/take_picture/interrupt",
            EVENT_QOS)
        self.__progress_msgs : Dict[str, SkillTakePictureProgress] = dict()
        self.__response_msgs : Dict[str, SkillTakePictureResponse] = dict()
        self.__events : Dict[str, threading.Event] = defaultdict(threading.Event)

        self.__status: SkillTakePictureStatus = None

    def start(self) -> str:
        request = SkillTakePictureRequest()
        request.id = new_id()
        self.__node.get_logger().debug(f"starting skill 'take_picture' with id {request.id}")
        self.__events[request.id].clear()
        self.__request_pub.publish(request)
        return request.id

    def interrupt(self, id: str):
        self.__node.get_logger().debug(f"interrupting skill 'take_picture' with id {id}")
        self.__interrupt_pub.publish(SkillInterrupt(id=id))

    def wait_result(self, id: str) -> SkillTakePictureResponse:
        self.__node.get_logger().debug(f"waiting skill 'take_picture' {id}")
        self.__events[id].wait()
        msg = self.__response_msgs[id]
        self.__node.get_logger().debug(f"got skill 'take_picture' response {msg}")
        return msg

    def __response_cb(self, msg: SkillTakePictureResponse):
        self.__node.get_logger().debug(f"received '' response {msg}")
        self.__response_msgs[msg.id] = msg
        self.__events[msg.id].set()

    
    def update_status(self, status: SkillTakePictureStatus):
        self.__status = status

    def status(self) -> SkillTakePictureStatus:
        return self.__status

    def is_ready(self) -> bool:
        return self.__status.state == SkillTakePictureStatus.READY

    def is_running(self) -> bool:
        return self.__status.state == SkillTakePictureStatus.RUNNING

    def is_interrupting(self) -> bool:
        return self.__status.state == SkillTakePictureStatus.INTERRUPTING





class Skills:
    def __init__(self, skillset_manager: str, node: rclpy.node.Node) -> None:
        
        self.__init_power = InitPowerSkill(skillset_manager, node)
        
        self.__safe_poweroff = SafePoweroffSkill(skillset_manager, node)
        
        self.__standup = StandupSkill(skillset_manager, node)
        
        self.__sitdown = SitdownSkill(skillset_manager, node)
        
        self.__go_to = GoToSkill(skillset_manager, node)
        
        self.__recharge = RechargeSkill(skillset_manager, node)
        
        self.__take_picture = TakePictureSkill(skillset_manager, node)
        self.__skills = ['init_power', 'safe_poweroff', 'standup', 'sitdown', 'go_to', 'recharge', 'take_picture']

    
    @property
    def init_power(self) -> InitPowerSkill:
        return self.__init_power
    
    @property
    def safe_poweroff(self) -> SafePoweroffSkill:
        return self.__safe_poweroff
    
    @property
    def standup(self) -> StandupSkill:
        return self.__standup
    
    @property
    def sitdown(self) -> SitdownSkill:
        return self.__sitdown
    
    @property
    def go_to(self) -> GoToSkill:
        return self.__go_to
    
    @property
    def recharge(self) -> RechargeSkill:
        return self.__recharge
    
    @property
    def take_picture(self) -> TakePictureSkill:
        return self.__take_picture
    

    def __getitem__(self, item):
        return getattr(self, item)

    def __iter__(self):
        return iter(self.__skills)

    def __len__(self):
        return len(self.__skills)

    def __nonzero__(self):
        return len(self.__skills) > 0
