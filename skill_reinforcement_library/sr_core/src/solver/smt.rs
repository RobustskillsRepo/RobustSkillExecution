use crate::expr::*;
use crate::model::*;
use std::collections::HashSet;
use crate::solver::*;
use z3_bridge::bridge::Z3Bridge;
use z3_bridge::result::SatResult;

pub trait ToSmt {
    fn to_smt(&self) -> String;
}

impl ToSmt for BoolExpr {
    fn to_smt(&self) -> String {
        match self {
            BoolExpr::Value(v) => v.to_string(),
            BoolExpr::Resource(rv) => naming(rv),
            BoolExpr::Not(expr) => format!("(not {})", expr.to_smt()),
            BoolExpr::And(lhs, rhs) => format!("(and {} {})", lhs.to_smt(), rhs.to_smt()),
            BoolExpr::Or(lhs, rhs) => format!("(or {} {})", lhs.to_smt(), rhs.to_smt()),
            BoolExpr::Iff(lhs, rhs) => format!("(= {} {})", lhs.to_smt(), rhs.to_smt()),
            BoolExpr::Implies(lhs, rhs) => {
                format!("(=> {} {})", lhs.to_smt(), rhs.to_smt())
            }
        }
    }
}

fn resource_unicity(resource: &Resource) -> State {
    let mut at_least_one = format!(
        "(or {}",
        resource
            .variables()
            .iter()
            .map(|v| naming(v))
            .collect::<Vec<_>>()
            .join(" ")
    );
    at_least_one.push_str(")");
    //
    let mut at_most_one = "(and".to_string();
    for (i, v) in resource.variables().iter().enumerate() {
        at_most_one.push_str(&format!(
            " (=> {} (not (or {})) )",
            naming(v),
            resource
                .variables()
                .iter()
                .enumerate()
                .filter_map(|(j, ov)| if i != j { Some(naming(ov)) } else { None })
                .collect::<Vec<_>>()
                .join(" ")
        ));
    }
    at_most_one.push_str(")");

    format!("(and {} {})", at_least_one, at_most_one)
}

fn declare_resource(bridge: &mut Z3Bridge, resource: &Resource) {
    bridge
        .add_comment(&format!("Resource: {}", resource.name()))
        .unwrap();
    for v in resource.variables().iter() {
        bridge.declare_const_bool(&naming(v)).unwrap();
    }
    bridge.assert(&resource_unicity(resource)).unwrap();
}

pub fn declare_resources(bridge: &mut Z3Bridge, model: &Model) {
    for resource in model.resources() {
        declare_resource(bridge, resource);
    }
}

pub fn check_state(
    model: &Model,
    state: &State,
    log_folder: &Option<&str>,
    message: Option<&str>,
) -> bool {
    let log_file = log_folder.map(|folder| {
        let msg = message.unwrap_or("unknown");
        format!("{}/check_{}.smt", folder, msg)
    });
    //
    let mut bridge = Z3Bridge::new("z3", vec!["-in"], log_file).unwrap();
    bridge.set_option("print-success", "false").unwrap();
    declare_resources(&mut bridge, model);
    //
    bridge.add_comment("State ∧ Global Invariant").unwrap();
    let guarded = state_and(state, &model.system_property().to_smt());
    bridge.assert(&guarded).unwrap();
    let result = bridge.check_sat().unwrap();
    bridge.exit().unwrap();
    //
    match result {
        SatResult::Sat => true,
        SatResult::Unsat => false,
        SatResult::Unknown => {
            panic!("Z3 returned Unknown");
        }
    }
}

pub fn compute_next_state(
    model: &Model,
    node_id: usize,
    state: &State,
    skill: &Skill,
    log_folder: &Option<&str>,
) -> State {
    let log_file =
        log_folder.map(|folder| format!("{}/next_{}_{}.smt", folder, node_id, skill.name()));
    //
    let mut bridge = Z3Bridge::new("z3", vec!["-in"], log_file).unwrap();
    bridge.set_option("print-success", "false").unwrap();
    //
    let prev = rename_to_prev(state, model);
    // resources
    declare_resources(&mut bridge, model);
    // exists
    let mut qe = "(exists (".to_string();
    for v in model.variables().iter() {
        let name = naming_prev(v);
        qe.push_str(&format!("({} Bool)", name));
    }
    qe.push_str(")\n  (and \n");
    qe.push_str("  ; Previous State ∧ Global Invariant\n");
    let prev_guarded = state_and(&prev, &rename_to_prev(&model.system_property().to_smt(), model));
    qe.push_str(&format!("    {}\n", prev_guarded));
    qe.push_str("  ; Skill Precondition\n");
    let precondition = rename_to_prev(&skill.precondition().to_smt(), model);
    qe.push_str(&format!("    {}\n", precondition));
    qe.push_str("  ; Skill Postcondition ∧ Global Invariant\n");
    let post_guarded = state_and(&skill.postcondition().to_smt(), &model.system_property().to_smt());
    qe.push_str(&format!("    {}\n", post_guarded));
    // frame axioms
    let unchanged = skill.postcondition().unchanged_resources(model);
    for resource in unchanged.iter() {
        qe.push_str(&format!("  ; Unchanged Resource {}\n", resource.name()));
        for v in resource.variables() {
            let current = naming(&v);
            let prev = naming_prev(&v);
            qe.push_str(&format!("    (= {} {})\n", prev, current));
        }
    }

    qe.push_str("  )\n)");
    bridge.assert(&qe).unwrap();
    //
    let result = bridge.apply(&tactics::qe_tactic()).unwrap();
    //
    bridge.exit().unwrap();
    //
    result
}

pub fn compute_prev_state(
    model: &Model,
    prev_state: &State,
    next_id: usize,
    next_state: &State,
    skill: &Skill,
    log_folder: &Option<&str>,
) -> State {
    let log_file =
        log_folder.map(|folder| format!("{}/prev_{}_{}.smt", folder, next_id, skill.name()));
    //
    let mut bridge = Z3Bridge::new("z3", vec!["-in"], log_file).unwrap();
    bridge.set_option("print-success", "false").unwrap();
    // resources
    declare_resources(&mut bridge, model);
    // add prev State
    bridge.add_comment("Prev State ∧ Global Invariant").unwrap();
    let prev_guarded = state_and(prev_state, &model.system_property().to_smt());
    bridge.assert(&prev_guarded).unwrap();
    // Skill preconditions
    bridge.add_comment("Skill Precondition").unwrap();
    let precondition = skill.precondition().to_smt();
    bridge.assert(&precondition).unwrap();
    // exists
    let mut qe = "(exists (".to_string();
    for v in model.variables().iter() {
        let name = naming_next(v);
        qe.push_str(&format!("({} Bool)", name));
    }
    qe.push_str(")\n  (and \n");
    //
    qe.push_str("  ; next State ∧ Global Invariant\n");
    let next = rename_to_next(next_state, model);
    let next_guarded = state_and(&next, &rename_to_next(&model.system_property().to_smt(), model));
    qe.push_str(&format!("    {}\n", next_guarded));
    // Skill Postcondition on next
    qe.push_str("  ; Skill Postcondition (next)\n");
    let post_next = rename_to_next(&skill.postcondition().to_smt(), model);
    qe.push_str(&format!("    {}\n", post_next));
    // Frame axioms: unchanged resources keep the same value from prev to next
    let unchanged = skill.postcondition().unchanged_resources(model);
    for resource in unchanged.iter() {
        qe.push_str(&format!("  ; Unchanged Resource {}\n", resource.name()));
        for v in resource.variables() {
            let prev = naming(&v);
            let next = naming_next(&v);
            qe.push_str(&format!("    (= {} {})\n", prev, next));
        }
    }
    qe.push_str("  )\n)");
    bridge.assert(&qe).unwrap();
    //
    let result = bridge.apply(&tactics::qe_tactic()).unwrap();
    //
    bridge.exit().unwrap();
    //
    result
}

// Consolidated node status check using a single incremental Z3 session.
// Returns whether the given state is empty, contains a solution for the objective,
// or is non-empty but contains no solution. This avoids recreating the solver twice.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum NodeStatus {
    Empty,
    HasSolution,
    NonEmptyNoSolution,
}

pub fn check_node_status(
    model: &Model,
    state: &State,
    objective: &str,
    log_folder: &Option<&str>,
    message: Option<&str>,
) -> NodeStatus {
    let log_file = log_folder.map(|folder| {
        let msg = message.unwrap_or("unknown");
        format!("{}/check_status_{}.smt", folder, msg)
    });
    // One solver session, two checks
    let mut bridge = Z3Bridge::new("z3", vec!["-in"], log_file).unwrap();
    bridge.set_option("print-success", "false").unwrap();
    declare_resources(&mut bridge, model);

    // Base: state ∧ Global Invariant
    bridge.add_comment("State ∧ Global Invariant").unwrap();
    let guarded = state_and(state, &model.system_property().to_smt());
    bridge.assert(&guarded).unwrap();
    match bridge.check_sat().unwrap() {
        SatResult::Unsat => {
            bridge.exit().unwrap();
            return NodeStatus::Empty;
        }
        SatResult::Unknown => panic!("Z3 returned Unknown"),
        SatResult::Sat => {
            // Now assert the objective and check for existence of a solution
            bridge.add_comment("Objective").unwrap();
            bridge.assert(objective).unwrap();
            match bridge.check_sat().unwrap() {
                SatResult::Sat => {
                    bridge.exit().unwrap();
                    NodeStatus::HasSolution
                }
                SatResult::Unsat => {
                    bridge.exit().unwrap();
                    NodeStatus::NonEmptyNoSolution
                }
                SatResult::Unknown => panic!("Z3 returned Unknown"),
            }
        }
    }
}

pub fn simplify_state(
    model: &Model,
    state: &State,
    log_folder: &Option<&str>,
    message: Option<&str>,
) -> State {
    let log_file = log_folder
        .map(|folder| format!("{}/simplify_{}.smt", folder, message.unwrap_or("unknown")));
    //
    let mut bridge = Z3Bridge::new("z3", vec!["-in"], log_file).unwrap();
    bridge.set_option("print-success", "false").unwrap();
    declare_resources(&mut bridge, model);
    //
    bridge.add_comment("Node State ∧ Global Invariant").unwrap();
    let guarded = state_and(state, &model.system_property().to_smt());
    bridge.assert(&guarded).unwrap();
    //
    let result = bridge.apply(&tactics::simplify_tactic()).unwrap();
    //
    bridge.exit().unwrap();
    //
    result
}

// Pretty-print a condition by simplifying it with the same tactic used elsewhere.
// This uses simplify_state, which applies PRE_SAT_TACTIC under the global invariant.
pub fn pretty_condition(model: &Model, condition: &str, log_folder: &Option<&str>) -> String {
    simplify_state(model, &condition.to_string(), log_folder, Some("recap_cond"))
}

// Identify which resources appear in a condition by scanning for variable names of the form
//   Resource__value
// Returns the unique set of resource names.
fn resources_in_condition(model: &Model, condition: &str) -> Vec<String> {
    let mut set: HashSet<String> = HashSet::new();
    for v in model.variables().iter() {
        let name = crate::solver::naming(v);
        if condition.contains(&name) {
            set.insert(v.resource().to_string());
        }
    }
    let mut v: Vec<String> = set.into_iter().collect();
    v.sort();
    v
}

// Enumerate concrete state spaces for a given condition restricted to the resources that
// appear in the condition. Uses an all-sat loop over the projection variables.
// Returns a vector of assignments; each assignment is a vec of (Resource, Value).
// If `max_states` is hit, enumeration stops early.
pub fn enumerate_state_spaces(
    model: &Model,
    condition: &str,
    log_folder: &Option<&str>,
    message: Option<&str>,
    max_states: usize,
) -> Vec<Vec<(String, String)>> {
    let proj_resources = resources_in_condition(model, condition);
    if proj_resources.is_empty() {
        // Nothing to project on — avoid enumerating the full space.
        return Vec::new();
    }

    let log_file = log_folder.map(|folder| {
        let msg = message.unwrap_or("recap_enum");
        format!("{}/enum_{}.smt", folder, msg)
    });

    let mut bridge = Z3Bridge::new("z3", vec!["-in"], log_file).unwrap();
    bridge.set_option("print-success", "false").unwrap();
    declare_resources(&mut bridge, model);

    // Base constraints: condition ∧ global invariant
    bridge.add_comment("Condition ∧ Global Invariant").unwrap();
    let guarded = state_and(&condition.to_string(), &model.system_property().to_smt());
    bridge.assert(&guarded).unwrap();

    let mut assignments: Vec<Vec<(String, String)>> = Vec::new();
    let mut iter: usize = 0;
    loop {
        match bridge.check_sat().unwrap() {
            SatResult::Unsat => break,
            SatResult::Unknown => panic!("Z3 returned Unknown"),
            SatResult::Sat => {
                // Extract the chosen value for each projected resource
                let mut asg: Vec<(String, String)> = Vec::new();
                let mut blocking_terms: Vec<String> = Vec::new();
                for r in model.resources().iter().filter(|r| proj_resources.contains(&r.name().to_string())) {
                    let mut chosen: Option<(String, String)> = None;
                    for rv in r.variables() {
                        let name = crate::solver::naming(&rv);
                        let val = bridge.eval(&name).unwrap();
                        if val.trim() == "true" {
                            chosen = Some((r.name().to_string(), rv.value().to_string()));
                            blocking_terms.push(name);
                            break;
                        }
                    }
                    // In case none evaluated to true (shouldn't happen due to unicity),
                    // fallback to pick the first value to avoid panics.
                    if let Some(c) = chosen {
                        asg.push(c);
                    } else if let Some(first) = r.values().first() {
                        let name = format!("{}__{}", r.name(), first);
                        asg.push((r.name().to_string(), first.clone()));
                        blocking_terms.push(name);
                    }
                }
                // Sort for stable display
                asg.sort_by(|a, b| a.0.cmp(&b.0));
                assignments.push(asg);

                iter += 1;
                if iter >= max_states {
                    break;
                }

                // Block the current projection to enumerate distinct state spaces
                let mut block = String::from("(not (and");
                for t in blocking_terms {
                    block.push_str(&format!(" {}", t));
                }
                block.push_str(") )");
                bridge.assert(&block).unwrap();
            }
        }
    }

    bridge.exit().unwrap();
    assignments
}

// Optional helper: check logical equivalence of two state formulas under the global invariant.
// Returns true if (A XOR B) ∧ GI is UNSAT, i.e., A and B are equivalent on the model's state space.
// Intended for debugging equivalence of canonicalizations; avoid using on hot paths.
pub fn states_equivalent(
    model: &Model,
    a: &State,
    b: &State,
    log_folder: &Option<&str>,
    message: Option<&str>,
) -> bool {
    let log_file = log_folder.map(|folder| {
        let msg = message.unwrap_or("equiv");
        format!("{}/equiv_{}.smt", folder, msg)
    });
    let mut bridge = Z3Bridge::new("z3", vec!["-in"], log_file).unwrap();
    bridge.set_option("print-success", "false").unwrap();
    declare_resources(&mut bridge, model);
    let gi = model.system_property().to_smt();
    // (A xor B) ∧ GI
    let xor = format!("(and (xor {} {}) {} )", a, b, gi);
    bridge.assert(&xor).unwrap();
    let res = bridge.check_sat().unwrap();
    bridge.exit().unwrap();
    matches!(res, SatResult::Unsat)
}
