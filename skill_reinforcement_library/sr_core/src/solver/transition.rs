use crate::solver::*;

#[derive(Debug, Clone)]
pub struct SolutionTransition {
    solved: State,
    skill: String,
    next: NodeRef,
}

impl SolutionTransition {
    pub fn new<T: Into<String>>(solved: State, skill: T, next: NodeRef) -> Self {
        Self {
            solved,
            skill: skill.into(),
            next,
        }
    }

    pub fn solved(&self) -> &State {
        &self.solved
    }

    pub fn skill(&self) -> &str {
        &self.skill
    }

    pub fn next(&self) -> NodeRef {
        self.next.clone()
    }

    pub fn add_solved(&mut self, solved: State) {
        // TODO: simplify
        self.solved = state_or(&self.solved, &solved);
    }
}
