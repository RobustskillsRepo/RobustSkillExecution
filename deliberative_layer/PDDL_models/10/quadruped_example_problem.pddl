(define (problem quadruped_example_problem)
    (:domain quadruped_example)

    (:objects
        L0 L1 L2 - Location
    )

    (:init
        ; resource initial states
        (resource_motor_state RESOURCE_MOTOR_STATE_STATE_OFF)
        (resource_spot_state RESOURCE_SPOT_STATE_STATE_SITTING)
        ; robot starts at L1
        (at_location L1)
        ; L1 is the charging station
        (charging_station L1)
    )

    (:goal
        (and
            ; take a picture at L2
            (picture_taken_at L2)
            ; recharge at L1
            (charged)
            ; return to L0
            (at_location L0)
            (resource_motor_state RESOURCE_MOTOR_STATE_STATE_OFF)
        )
    )

)
