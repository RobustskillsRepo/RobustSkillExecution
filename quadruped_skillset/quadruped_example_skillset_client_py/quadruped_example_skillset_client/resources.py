from importlib import resources
from quadruped_example_skillset_interfaces.msg import SkillsetStatus

class Resources:
    def __init__(self) -> None:
        self.__motor_state : str = "Off"
        self.__spot_state : str = "Sitting"
        self.__resources = ['motor_state', 'spot_state']
    
    @property
    def motor_state(self) -> str:
        return self.__motor_state
    
    @property
    def spot_state(self) -> str:
        return self.__spot_state
    

    def __getitem__(self, item):
        return getattr(self, item)

    def __iter__(self):
        return iter(self.__resources)

    def __len__(self):
        return len(self.__resources)

    def __nonzero__(self):
        return len(self.__resources) > 0
    
    def update_status(self, status: SkillsetStatus) -> None:
        for r in status.resources:
            if r.name == 'motor_state':
                self.__motor_state = r.state
            elif r.name == 'spot_state':
                self.__spot_state = r.state
            
