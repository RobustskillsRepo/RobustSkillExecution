use crate::model::Skill;
use crate::solver::*;
use std::collections::{HashMap, HashSet};

#[derive(Debug, Clone)]
pub struct Node {
    previous: Option<NodeRef>,
    id: usize,
    cost: f64,
    //
    skill: Option<String>,
    //
    unsolved: State, // unsolved state
    solved: State,
    // Canonical string of the current unsolved state (after simplify_state)
    canon: String,
    //
    solutions: Vec<SolutionTransition>,
    //
    next_nodes: Vec<NodeRef>,
    // Skills already expanded from this node (even if later pruned)
    expanded_skills: HashSet<String>,
    // Per-skill expansion info: last parent unsolved signature and optional child node
    expansions: HashMap<String, ExpansionInfo>,
    // Number of times this node was refreshed due to parent change
    updated_times: u32,
}

#[derive(Debug, Clone)]
pub struct ExpansionInfo {
    pub parent_sig: u64,
    pub child: Option<NodeRef>,
}

impl Node {
    pub fn initial(id: usize, state: State, cost: f64) -> Self {
        Self {
            previous: None,
            id,
            cost,
            skill: None,
            unsolved: state,
            solved: state_false(),
            canon: String::new(),
            solutions: Vec::new(),
            next_nodes: Vec::new(),
            expanded_skills: HashSet::new(),
            expansions: HashMap::new(),
            updated_times: 0,
        }
    }

    pub fn is_local_solution(&self) -> bool {
        // A node has a local solution part if its `solved` field is not the boolean false
        self.solved != state_false()
    }

    pub fn new<T: Into<String>>(
        previous: Option<NodeRef>,
        id: usize,
        skill: T,
        state: State,
        cost: f64,
    ) -> Self {
        Self {
            previous,
            id,
            cost,
            skill: Some(skill.into()),
            unsolved: state,
            solved: state_false(),
            canon: String::new(),
            solutions: Vec::new(),
            next_nodes: Vec::new(),
            expanded_skills: HashSet::new(),
            expansions: HashMap::new(),
            updated_times: 0,
        }
    }

    pub fn previous(&self) -> Option<NodeRef> {
        self.previous.clone()
    }

    pub fn id(&self) -> usize {
        self.id
    }

    pub fn cost(&self) -> f64 {
        self.cost
    }

    pub fn skill(&self) -> Option<&str> {
        self.skill.as_deref()
    }

    pub fn unsolved(&self) -> &State {
        &self.unsolved
    }

    pub fn set_unsolved(&mut self, state: State) {
        self.unsolved = state;
    }

    pub fn set_canon<T: Into<String>>(&mut self, s: T) {
        self.canon = s.into();
    }

    pub fn canon(&self) -> &str {
        &self.canon
    }

    //------------------------- Skills -------------------------//

    pub fn available_skills<'a>(&self, others: Vec<&'a Skill>) -> Vec<&'a Skill> {
        others
            .into_iter()
            .filter(|skill| {
                // Do not propose skills that already have a recorded solution transition
                // We allow previously expanded skills to be revisited (to refresh) when parent changes.
                !self
                    .solutions
                    .iter()
                    .any(|solution| solution.skill() == skill.name())
            })
            .collect()
    }

    //------------------------- Solutions -------------------------//

    pub fn add_local_solution(&mut self, state: State) {
        // Accumulate local solutions: solved := solved ∨ state
        if self.solved == state_false() {
            self.solved = state;
        } else {
            self.solved = state_or(&self.solved, &state);
        }
    }

    pub fn solved(&self) -> &State {
        &self.solved
    }

    pub fn add_solution_transition(&mut self, solved: State, skill: String, next: NodeRef) {
        for ts in self.solutions.iter_mut() {
            if ts.skill() == skill {
                ts.add_solved(solved);
                return;
            }
        }
        let transition = SolutionTransition::new(solved, skill, next);
        self.solutions.push(transition);
    }

    //------------------------- Transitions -------------------------//

    pub fn next_nodes(&self) -> &Vec<NodeRef> {
        &self.next_nodes
    }

    pub fn add_next_node(&mut self, next: NodeRef) {
        self.next_nodes.push(next);
    }

    pub fn clear_next_nodes(&mut self) {
        self.next_nodes.clear();
    }

    pub fn solution_nodes(&self) -> Vec<NodeRef> {
        self.solutions
            .iter()
            .map(|solution| solution.next())
            .collect()
    }

    pub fn solutions(&self) -> &Vec<SolutionTransition> {
        &self.solutions
    }

    //------------------------- Expansion bookkeeping -------------------------//

    pub fn mark_expanded<T: AsRef<str>>(&mut self, skill: T) {
        self.expanded_skills.insert(skill.as_ref().to_string());
    }

    pub fn set_expansion(&mut self, skill: &str, parent_sig: u64, child: Option<NodeRef>) {
        self.expansions.insert(
            skill.to_string(),
            ExpansionInfo {
                parent_sig,
                child,
            },
        );
    }

    pub fn expansion_parent_sig(&self, skill: &str) -> Option<u64> {
        self.expansions.get(skill).map(|e| e.parent_sig)
    }

    pub fn expansion_child(&self, skill: &str) -> Option<NodeRef> {
        self.expansions
            .get(skill)
            .and_then(|e| e.child.as_ref().map(|c| c.clone()))
    }

    pub fn ensure_next_contains(&mut self, child: NodeRef) {
        let id = child.borrow().id();
        if !self
            .next_nodes
            .iter()
            .any(|n| n.borrow().id() == id)
        {
            self.next_nodes.push(child);
        }
    }

    pub fn remove_next_child_by_id(&mut self, id: usize) {
        self.next_nodes.retain(|n| n.borrow().id() != id);
    }

    pub fn unsolved_signature(&self) -> u64 {
        use std::collections::hash_map::DefaultHasher;
        use std::hash::{Hash, Hasher};
        let mut hasher = DefaultHasher::new();
        self.unsolved.hash(&mut hasher);
        hasher.finish()
    }

    //------------------------- Update tracking -------------------------//

    pub fn mark_updated(&mut self) {
        self.updated_times = self.updated_times.saturating_add(1);
    }

    pub fn is_updated(&self) -> bool {
        self.updated_times > 0
    }

    pub fn updated_times(&self) -> u32 {
        self.updated_times
    }
}

impl std::fmt::Display for Node {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "Node id: {} {{\n  cost: {}\n", self.id, self.cost)?;
        write!(
            f,
            "  previous: {}\n",
            self.previous.as_ref().map(|n| n.borrow().id()).unwrap_or(0)
        )?;
        write!(
            f,
            "  skill: {}\n",
            self.skill.as_deref().unwrap_or("initial")
        )?;
        write!(f, "  solved: {}\n", self.solved)?;
        write!(f, "  unsolved: {}\n", self.unsolved)?;

        write!(f, "  solutions: [\n")?;
        for solution in self.solutions.iter() {
            write!(
                f,
                "    skill: {}, solved: {}, next_id: {}",
                solution.skill(),
                solution.solved(),
                solution.next().borrow().id()
            )?;
            writeln!(f, ",")?;
        }
        write!(f, "  ],\n  next_nodes: [\n")?;
        for node in self.next_nodes.iter() {
            write!(
                f,
                "    skill: {}, next_id: {}",
                node.borrow().skill().unwrap_or("initial"),
                node.borrow().id()
            )?;
            writeln!(f, ",")?;
        }
        writeln!(f, "  ]\n}}")?;
        Ok(())
    }
}
