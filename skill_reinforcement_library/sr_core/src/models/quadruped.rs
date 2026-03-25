use crate::expr::ImpliesTrait;
use crate::model::*;

/// Build the quadruped demo model used for robustification examples.
pub fn build_quadruped_model() -> Model {
    let mut model = Model::default();
    // motor_state
    let mut motor_state = Resource::empty("motor_state");
    let power_on = motor_state.add_value("On");
    let power_off = motor_state.add_value("Off");
    model.add_resource(motor_state);
    // spot_state
    let mut spot_state = Resource::empty("spot_state");
    let sitting = spot_state.add_value("Sitting");
    let standing = spot_state.add_value("Standing");
    model.add_resource(spot_state);

    // System property: motor_state.off => spot_state.sit
    model.set_system_property(power_off.expr().implies(sitting.expr()));

    // Start motor_state
    let init_power = Skill::new(
        "init_power",
        power_off.expr() & sitting.expr(),
        power_on.expr() & sitting.expr(),
        1.0,
    );
    model.add_skill(init_power);
    // Safe power off
    let safe_poweroff = Skill::new(
        "safe_poweroff",
        power_on.expr() & sitting.expr(),
        power_off.expr() & sitting.expr(),
        1.0,
    );
    model.add_skill(safe_poweroff);
    // Stand up
    let standup = Skill::new(
        "standup",
        power_on.expr() & sitting.expr(),
        power_on.expr() & standing.expr(),
        1.0,
    );
    model.add_skill(standup);
    // Sit down
    let sitdown = Skill::new(
        "sitdown",
        power_on.expr() & standing.expr(),
        power_on.expr() & sitting.expr(),
        1.0,
    );
    model.add_skill(sitdown);
    // Skill go_to
    let go_to = Skill::new(
        "go_to",
        power_on.expr() & standing.expr(),
        power_on.expr() & standing.expr(),
        1.0,
    );
    model.add_skill(go_to);
    // Skill recharge
    let recharge = Skill::new("recharge", sitting.expr(), sitting.expr(), 1.0);
    model.add_skill(recharge);
    // Skill take_picture
    let take_picture = Skill::new("take_picture", power_on.expr(), power_on.expr(), 1.0);
    model.add_skill(take_picture);

    model
}

