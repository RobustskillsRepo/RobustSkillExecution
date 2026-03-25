pub mod smt;
pub use smt::*;

pub mod tactics;

pub mod node;
pub use node::*;

pub mod transition;
pub use transition::*;

pub mod solver;
pub use solver::*;

mod display;

use std::cell::RefCell;
use std::rc::Rc;

pub type NodeRef = Rc<RefCell<Node>>;

pub fn new_node_ref(node: Node) -> NodeRef {
    Rc::new(RefCell::new(node))
}

pub type State = String;

pub fn state_false() -> State {
    "false".to_string()
}

pub fn state_true() -> State {
    "true".to_string()
}

pub fn state_not(s: &State) -> State {
    format!("(not {})", s)
}

pub fn state_and(s1: &State, s2: &State) -> State {
    format!("(and {} {})", s1, s2)
}

pub fn state_or(s1: &State, s2: &State) -> State {
    format!("(or {} {})", s1, s2)
}

pub fn naming(var: &crate::model::ResourceValue) -> String {
    format!("{}__{}", var.resource(), var.value())
}

pub fn naming_prev(var: &crate::model::ResourceValue) -> String {
    format!("{}__{}__prev", var.resource(), var.value())
}

pub fn naming_next(var: &crate::model::ResourceValue) -> String {
    format!("{}__{}__next", var.resource(), var.value())
}

pub fn rename_to_next(state: &State, model: &crate::model::Model) -> State {
    let mut state = state.clone();
    for v in model.variables().iter() {
        let name = naming(v);
        let next = naming_next(v);
        state = state.replace(&name, &next);
    }
    state
}

pub fn rename_to_prev(state: &State, model: &crate::model::Model) -> State {
    let mut state = state.clone();
    for v in model.variables().iter() {
        let name = naming(v);
        let prev = naming_prev(v);
        state = state.replace(&name, &prev);
    }
    state
}

// Convenience wrappers for renaming SMT strings that represent formulas
pub fn rename_state_to_next(state: &State, model: &crate::model::Model) -> State {
    rename_to_next(state, model)
}

pub fn rename_state_to_prev(state: &State, model: &crate::model::Model) -> State {
    rename_to_prev(state, model)
}

// Ensure the log folder exists; do not delete existing content.
// Safe to call at the start of each run; failures are ignored to avoid
// impacting solver execution when logging is optional. Prefer passing a
// unique per-run subfolder to avoid mixing logs across runs.
pub fn prepare_log_folder(log_folder: &Option<&str>) {
    if let Some(folder) = log_folder {
        let path = std::path::Path::new(folder);
        // Best-effort creation only (no deletion)
        let _ = std::fs::create_dir_all(path);
    }
}
