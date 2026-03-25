(define (domain quadruped_example)
    (:requirements :strips :typing)
    (:types
        Location
        Float
        RESOURCE_MOTOR_STATE_STATE
        RESOURCE_SPOT_STATE_STATE
    )
    (:constants
        RESOURCE_MOTOR_STATE_STATE_OFF - RESOURCE_MOTOR_STATE_STATE
        RESOURCE_MOTOR_STATE_STATE_ON - RESOURCE_MOTOR_STATE_STATE
        RESOURCE_SPOT_STATE_STATE_SITTING - RESOURCE_SPOT_STATE_STATE
        RESOURCE_SPOT_STATE_STATE_STANDING - RESOURCE_SPOT_STATE_STATE
    )
    (:predicates
        (resource_motor_state ?state - RESOURCE_MOTOR_STATE_STATE)
        (resource_spot_state ?state - RESOURCE_SPOT_STATE_STATE)
        (at_location ?loc - Location)
        (picture_taken_at ?loc - Location)
        (charged)
        (charging_station ?loc - Location)
    )

    (:action init_power
        :parameters (
        )
        :precondition (and
        )
        :effect (and
            ; start effect
            ; mode 'is_poweredon' effect
            (not (resource_motor_state RESOURCE_MOTOR_STATE_STATE_OFF))
            (resource_motor_state RESOURCE_MOTOR_STATE_STATE_ON)
            ; output
        )
    )

    (:action safe_poweroff
        :parameters (
        )
        :precondition (and
            ; precond is_sitting
            (resource_spot_state RESOURCE_SPOT_STATE_STATE_SITTING)
            ; precond is_powered
            (resource_motor_state RESOURCE_MOTOR_STATE_STATE_ON)
        )
        :effect (and
            ; start effect
            ; mode 'is_poweredoff' effect
            (not (resource_motor_state RESOURCE_MOTOR_STATE_STATE_ON))
            (resource_motor_state RESOURCE_MOTOR_STATE_STATE_OFF)
            ; output
        )
    )

    (:action standup
        :parameters (
        )
        :precondition (and
            ; precond is_sitting
            (resource_spot_state RESOURCE_SPOT_STATE_STATE_SITTING)
            ; precond is_powered
            (resource_motor_state RESOURCE_MOTOR_STATE_STATE_ON)
        )
        :effect (and
            ; start effect
            ; mode 'is_standing' effect
            (not (resource_spot_state RESOURCE_SPOT_STATE_STATE_SITTING))
            (resource_spot_state RESOURCE_SPOT_STATE_STATE_STANDING)
            ; output
        )
    )

    (:action sitdown
        :parameters (
        )
        :precondition (and
            ; precond is_standing
            (resource_spot_state RESOURCE_SPOT_STATE_STATE_STANDING)
            ; precond is_powered
            (resource_motor_state RESOURCE_MOTOR_STATE_STATE_ON)
        )
        :effect (and
            ; start effect
            ; mode 'is_sitting' effect
            (not (resource_spot_state RESOURCE_SPOT_STATE_STATE_STANDING))
            (resource_spot_state RESOURCE_SPOT_STATE_STATE_SITTING)
            ; output
        )
    )

    (:action go_to
        :parameters (
            ?i_target - Location
            ?from - Location
        )
        :precondition (and
            ; precond is_standing
            (resource_spot_state RESOURCE_SPOT_STATE_STATE_STANDING)
            ; precond is_powered
            (resource_motor_state RESOURCE_MOTOR_STATE_STATE_ON)
            ; at current location
            (at_location ?from)
        )
        :effect (and
            ; start effect
            ; mode 'is_arrived' effect
            ; move to target location
            (not (at_location ?from))
            (at_location ?i_target)
            ; output
        )
    )

    (:action recharge
        :parameters (
            ?at_loc - Location
        )
        :precondition (and
            ; precond is_sitting
            (resource_spot_state RESOURCE_SPOT_STATE_STATE_SITTING)
            ; must be at location
            (at_location ?at_loc)
            ; must be at charging station
            (charging_station ?at_loc)
        )
        :effect (and
            ; start effect
            ; mode 'has_charged' effect
            (charged)
            ; output
        )
    )

    (:action take_picture
        :parameters (
            ?at_loc - Location
        )
        :precondition (and
            ; must be at location
            (at_location ?at_loc)
        )
        :effect (and
            ; start effect
            ; mode 'pic_taken' effect
            (picture_taken_at ?at_loc)
            ; output
        )
    )

)
