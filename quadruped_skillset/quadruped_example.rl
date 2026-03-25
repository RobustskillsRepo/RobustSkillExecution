type {
    Location
    Float
}


skillset quadruped_example {

    //===========================================

    data {
        location: Location
        battery: Float
    }

    resource {
        motor_state {
            state { Off On }
            initial Off
            transition all
        }  
        spot_state {
            state { Sitting Standing }
            initial Sitting
            transition all
        }
    }

    //===========================================

    event {
        set_standing {
            guard spot_state == Sitting
            effect spot_state -> Standing
        }
        set_sitting {
            guard spot_state == Standing
            effect spot_state -> Sitting
        }
        set_poweroff {
            guard motor_state == On
            effect {
                spot_state -> Sitting
                motor_state -> Off
            }
        }
        set_poweron {
            guard motor_state == Off
            effect motor_state -> On
        }
    }

    //===========================================
    
    skill init_power {
        precondition {
            is_sitting: spot_state == Sitting
            is_not_powered : motor_state == Off
        }
        success is_poweredon {
            effect motor_state -> On
        }
        failure couldnot_On {}
    }

    skill safe_poweroff {
        precondition {
            is_sitting: spot_state == Sitting
            is_powered : motor_state == On
        }
        success is_poweredoff {
            effect motor_state -> Off
        }
        failure couldnot_Off {}
    }

    //===========================================

    skill standup {
        precondition {
            is_sitting: spot_state == Sitting
            is_powered : motor_state == On
        }
        invariant {
            is_powered {
                guard motor_state == On
            }
        }
        success is_standing {
            effect spot_state -> Standing
        }
        failure couldnot_stand {}
    }

    skill sitdown {
        precondition {
            is_standing: spot_state == Standing
            is_powered  : motor_state == On
        }
        invariant {
            is_powered {
                guard motor_state == On
            }
        }
        success is_sitting {
            effect spot_state -> Sitting
        }
        failure couldnot_sit {}
    }

    //===========================================

    skill go_to {
        input target: Location
        precondition {
            is_standing: spot_state == Standing
            is_powered: motor_state == On
        }
        invariant {
            is_standing {
                guard spot_state == Standing
            }
            is_powered {
                guard motor_state == On
            }
        }
        success is_arrived {}
        failure not_arrived {}
    }

    skill recharge {
        precondition {
            is_sitting: spot_state == Sitting
        }
        invariant {
            is_sitting {
                guard spot_state == Sitting
            }
        }
        success has_charged {}
        failure couldnot_charge {}
    }

    //===========================================
    
    skill take_picture {
        success pic_taken {}
        failure pic_failed {}
    }
    
    //===========================================
}
