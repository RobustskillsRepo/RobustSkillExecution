use crate::expr::BoolExpr;
use crate::model::*;
use crate::solver::*;
use std::collections::{HashMap, HashSet};
use std::fs::File;
use std::io::Write;
use std::time::{Duration, Instant};

const MAX_REFRESHES_PER_NODE: u32 = 4;
/// Safety: prevent runaway when SMT canonical forms vary and graph explodes.
const MAX_SOLVE_ITERATIONS: u64 = 500_000;

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum Verbosity {
    Quiet,
    Normal,
    Debug,
}

pub struct Solver<'a> {
    model: &'a Model,
    skill: &'a Skill,
    max_cost: f64,
    //
    root: Option<NodeRef>,
    // Keep track of all created nodes for summary/reporting even if edges are pruned later
    all_nodes: Vec<NodeRef>,
    // Elapsed runtime of the last solve
    elapsed: Option<Duration>,
    // Last used log folder for optional recap simplification
    last_log_folder: Option<String>,
    // Recap: cap number of enumerated concrete states per path
    enum_cap: usize,
    // Controls console verbosity
    verbosity: Verbosity,
    // Global visited-state map to deduplicate nodes across parents/paths
    visited: HashMap<String, NodeRef>,
    // Optional cap on number of visited states retained
    visited_cap: Option<usize>,
    // Parent adjacency: child_id -> list of (parent_node, skill_name)
    parents: HashMap<usize, Vec<(NodeRef, String)>>,
}

impl Solver<'_> {
    pub fn new<'a>(model: &'a Model, skill: &'a Skill, max_cost: f64) -> Solver<'a> {
        Solver {
            model,
            skill,
            max_cost,
            //
            root: None,
            all_nodes: Vec::new(),
            elapsed: None,
            last_log_folder: None,
            enum_cap: 5,
            verbosity: Verbosity::Quiet,
            visited: HashMap::new(),
            visited_cap: None,
            parents: HashMap::new(),
        }
    }

    pub fn set_enum_cap(&mut self, cap: usize) {
        self.enum_cap = cap.max(1);
    }

    pub fn set_verbosity(&mut self, verbosity: Verbosity) {
        self.verbosity = verbosity;
    }

    /// Optionally limit the number of visited states retained for deduplication.
    pub fn set_visited_cap(&mut self, cap: usize) {
        self.visited_cap = Some(cap.max(1));
    }

    pub fn objective(&self) -> &BoolExpr {
        // Objective is the target skill precondition: reach a state where the target can be fired
        self.skill.precondition()
    }

    /// For Display impl (in display module).
    pub(crate) fn display_skill(&self) -> &Skill {
        self.skill
    }

    pub(crate) fn display_max_cost(&self) -> f64 {
        self.max_cost
    }

    pub(crate) fn display_last_log_folder(&self) -> Option<&str> {
        self.last_log_folder.as_deref()
    }

    pub(crate) fn display_model(&self) -> &Model {
        self.model
    }

    pub(crate) fn display_enum_cap(&self) -> usize {
        self.enum_cap
    }

    pub(crate) fn display_elapsed(&self) -> Option<Duration> {
        self.elapsed
    }

    //------------------------- Solver -------------------------//

    pub fn solve(&mut self, log_folder: &Option<&str>) {
        // Ensure a clean log directory for this run, if logging is enabled
        crate::solver::prepare_log_folder(log_folder);
        let start_time = Instant::now();
        let mut id = 0;
        // Remember log folder for recap pretty printing
        self.last_log_folder = log_folder.map(|s| s.to_string());

        //---------- Skills used for Reinforcement ----------//
        let mut other_skills: Vec<&Skill> = self
            .model
            .skills()
            .iter()
            .filter(|s| s.name() != self.skill.name())
            .filter(|s| !s.is_transparent())  // Exclude transparent skills
            .collect();
        other_skills.sort_by(|a, b| {
            a.cost()
                .partial_cmp(&b.cost())
                .unwrap_or(std::cmp::Ordering::Equal)
        });

        //---------- Initial Node ----------//
        // Initial formula to reinforce: not pre, pruned by global invariant
        let initial_formula = !self.skill.precondition()
            & self.model.system_property().clone();
        let initial_state = simplify_state(
            self.model,
            &initial_formula.to_smt(),
            log_folder,
            Some("initial_state"),
        );
    // Initial Node
    let initial_node = new_node_ref(Node::initial(id, initial_state.clone(), 0.0));
    initial_node.borrow_mut().set_canon(initial_state.clone());
        self.root = Some(initial_node.clone());
        // record initial
        self.all_nodes.push(initial_node.clone());
        // seed visited with the initial state (already simplified)
        self.visited.insert(initial_state, initial_node.clone());
        //
        let mut todo = Vec::new();
        let mut in_todo: HashSet<usize> = HashSet::new();
        todo.push(initial_node);
        in_todo.insert(0);

        let debug = matches!(self.verbosity, Verbosity::Debug);
        let mut iterations: u64 = 0;

        while !todo.is_empty() {
            iterations += 1;
            if iterations > MAX_SOLVE_ITERATIONS {
                break;
            }
            let current_node = todo.pop().unwrap();
            let current_id = current_node.borrow().id();
            in_todo.remove(&current_id);
            let current_cost = current_node.borrow().cost();

            if debug {
                println!(
                    "[backward::solve] node_id={} cost={:.3} todo_len={} visited_len={}",
                    current_id,
                    current_cost,
                    todo.len(),
                    self.visited.len()
                );
            }

            // Consolidated status check: empty / has solution / non-empty without solution
            // Work on the effective state: mask out already-solved region to avoid re-solving it
            let current_state = if current_node.borrow().is_local_solution() {
                state_and(
                    current_node.borrow().unsolved(),
                    &state_not(current_node.borrow().solved()),
                )
            } else {
                current_node.borrow().unsolved().clone()
            };

            let status = check_node_status(
                self.model,
                &current_state,
                &self.objective().to_smt(),
                log_folder,
                Some(&format!("node_{}", current_id)),
            );

            if matches!(status, NodeStatus::Empty) {
                continue;
            }

            if matches!(status, NodeStatus::HasSolution) {
                let solution_state = state_and(&current_state, &self.objective().to_smt());
                // solution
                let solution_state = simplify_state(
                    self.model,
                    &solution_state,
                    log_folder,
                    Some(&format!("solution_{}", current_id)),
                );
                current_node
                    .borrow_mut()
                    .add_local_solution(solution_state.clone());

                //---------- Backward accounting across all parents (DAG) ----------//
                // Accumulate solution transitions backward through all recorded parents using DFS.
                use std::collections::HashSet;
                const MAX_BACKPROP_STACK: usize = 50_000;
                let mut stack: Vec<(NodeRef, State)> = Vec::new();
                let mut seen_edges: HashSet<(usize, usize, String)> = HashSet::new();
                stack.push((current_node.clone(), solution_state.clone()));
                let mut backprop_capped = false;
                while let Some((child_node, child_solution)) = stack.pop() {
                    if backprop_capped {
                        break;
                    }
                    let child_id = child_node.borrow().id();
                    // For each parent edge recorded for this child, compute its preimage
                    if let Some(edges) = self.parents.get(&child_id) {
                        for (parent_ref, skill_name) in edges.iter() {
                            let parent_id = parent_ref.borrow().id();
                            let edge_key = (parent_id, child_id, skill_name.clone());
                            if !seen_edges.insert(edge_key) {
                                continue;
                            }
                            if seen_edges.len() > MAX_BACKPROP_STACK {
                                backprop_capped = true;
                                break;
                            }
                            // Find the skill by name
                            let skill = self
                                .model
                                .skills()
                                .iter()
                                .find(|s| s.name() == skill_name.as_str())
                                .unwrap();
                            let prev_unsolved = parent_ref.borrow().unsolved().clone();
                            let prev_solution = compute_prev_state(
                                self.model,
                                &prev_unsolved,
                                child_id,
                                &child_solution,
                                skill,
                                log_folder,
                            );
                            // Record solution transition but do not touch parent_ref.unsolved or next_nodes
                            parent_ref.borrow_mut().add_solution_transition(
                                prev_solution.clone(),
                                skill_name.clone(),
                                child_node.clone(),
                            );
                            // Continue traversing upward
                            stack.push((parent_ref.clone(), prev_solution));
                        }
                    }
                }
                
                // After back-propagation, check if root is now fully solved
                if let Some(root) = &self.root {
                    let root_borrow = root.borrow();
                    let root_effective = state_and(
                        root_borrow.unsolved(),
                        &state_not(root_borrow.solved()),
                    );
                    drop(root_borrow);
                    let root_empty = !check_state(
                        self.model,
                        &root_effective,
                        log_folder,
                        Some("check_root_fully_solved"),
                    );
                    if root_empty {
                        // Root fully solved, stop search
                        break;
                    }
                }
            } else {
                // Expand or refresh next node per skill
                let available_skills = current_node.borrow().available_skills(other_skills.clone());
                // Snapshot current parent's effective signature (masked by already-solved if any)
                let parent_unsolved_masked = if current_node.borrow().is_local_solution() {
                    state_and(
                        current_node.borrow().unsolved(),
                        &state_not(current_node.borrow().solved()),
                    )
                } else {
                    current_node.borrow().unsolved().clone()
                };
                use std::collections::hash_map::DefaultHasher;
                use std::hash::{Hash, Hasher};
                let mut hasher = DefaultHasher::new();
                parent_unsolved_masked.hash(&mut hasher);
                let parent_sig = hasher.finish();
                for &skill in available_skills.iter() {
                    // 1: check cost
                    let next_cost = current_cost + skill.cost();
                    if next_cost > self.max_cost {
                        continue;
                    }

                    // If we have an expansion record and the parent hasn't changed, skip recomputation
                    let prev_sig = current_node.borrow().expansion_parent_sig(skill.name());
                    if let Some(ps) = prev_sig {
                        if ps == parent_sig {
                            // up-to-date; nothing to do
                            continue;
                        }
                    }

                    // 2: check precondition on current parent state, masked by parent.solved if any
                    // parent_unsolved_masked already computed above
                    // Check precondition under masked state
                    if !check_state(
                        self.model,
                        &state_and(&parent_unsolved_masked, &skill.precondition().to_smt()),
                        log_folder,
                        Some(&format!(
                            "node_{}_skill_{}_precondition",
                            current_id,
                            skill.name()
                        )),
                    ) {
                        // update expansion record with new parent sig but no child update
                        let existing_child = current_node.borrow().expansion_child(skill.name());
                        current_node
                            .borrow_mut()
                            .set_expansion(skill.name(), parent_sig, existing_child);
                        continue;
                    }

                    // 3: compute next State (QE) under masked parent state
                    let next_state = compute_next_state(
                        self.model,
                        current_id,
                        &parent_unsolved_masked,
                        skill,
                        log_folder,
                    );

                    // 4: check next state satisfiable
                    let sat = check_state(
                        self.model,
                        &next_state,
                        log_folder,
                        Some(&format!("node_{}_next_{}", current_id, skill.name())),
                    );

                    // Existing child?
                    let maybe_child = current_node.borrow().expansion_child(skill.name());
                    if let Some(child) = maybe_child {
                        if sat {
                            // Canonicalize and update visited map
                            let canon = simplify_state(
                                self.model,
                                &next_state,
                                log_folder,
                                Some(&format!("canon_{}_{}", current_id, skill.name())),
                            );
                            let old_canon = child.borrow().canon().to_string();
                            let state_changed = canon != old_canon;
                            // Refresh existing child with new state
                            child.borrow_mut().set_unsolved(next_state.clone());
                            child.borrow_mut().set_canon(canon.clone());
                            // Ensure it's present in transitions
                            current_node.borrow_mut().ensure_next_contains(child.clone());
                            // Mark child as updated due to refresh
                            child.borrow_mut().mark_updated();
                            let times = child.borrow().updated_times();
                            // Update expansion record with new parent signature
                            current_node
                                .borrow_mut()
                                .set_expansion(skill.name(), parent_sig, Some(child.clone()));
                            // Update visited mapping: remove old canon to avoid unbounded growth
                            // when SMT returns different canonical forms for the same logical state
                            if state_changed {
                                self.visited.remove(&old_canon);
                            }
                            self.visited.insert(canon, child.clone());
                            // Ensure parents adjacency contains this edge
                            let cid = child.borrow().id();
                            let entry = self.parents.entry(cid).or_insert_with(Vec::new);
                            if !entry.iter().any(|(p, s)| p.borrow().id() == current_id && s == skill.name()) {
                                entry.push((current_node.clone(), skill.name().to_string()));
                            }
                            // Re-queue only if canonical state changed and refresh count is bounded
                            if state_changed && times <= MAX_REFRESHES_PER_NODE && in_todo.insert(cid) {
                                todo.push(child);
                            }
                        } else {
                            // Remove child if now unsat
                            let child_id = child.borrow().id();
                            current_node.borrow_mut().remove_next_child_by_id(child_id);
                            current_node
                                .borrow_mut()
                                .set_expansion(skill.name(), parent_sig, None);
                            // Remove parents adjacency edge
                            if let Some(edges) = self.parents.get_mut(&child_id) {
                                edges.retain(|(p, s)| !(p.borrow().id() == current_id && s == skill.name()));
                                if edges.is_empty() {
                                    self.parents.remove(&child_id);
                                }
                            }
                        }
                        // done with this skill
                        continue;
                    }

                    // No existing child: create if sat
                    if !sat {
                        // Record that we've seen this parent version even if no child is created
                        current_node
                            .borrow_mut()
                            .set_expansion(skill.name(), parent_sig, None);
                        continue;
                    }

                    // Sat and no existing child: canonicalize and try to reuse a visited node
                    let canon = simplify_state(
                        self.model,
                        &next_state,
                        log_folder,
                        Some(&format!("canon_{}_{}", current_id, skill.name())),
                    );
                    if let Some(existing) = self.visited.get(&canon) {
                        // Reuse existing node: link and update expansion record; do not create a new node
                        current_node
                            .borrow_mut()
                            .ensure_next_contains(existing.clone());
                        current_node
                            .borrow_mut()
                            .set_expansion(skill.name(), parent_sig, Some(existing.clone()));
                        // Record parents adjacency
                        let eid = existing.borrow().id();
                        let entry = self.parents.entry(eid).or_insert_with(Vec::new);
                        if !entry.iter().any(|(p, s)| p.borrow().id() == current_id && s == skill.name()) {
                            entry.push((current_node.clone(), skill.name().to_string()));
                        }
                        // Do not push to todo; existing node will be/has been processed
                        continue;
                    }

                    // Cap total nodes: do not create more once at visited_cap (ensures termination
                    // when SMT canonical forms are not unique and graph would otherwise grow)
                    let can_insert = match self.visited_cap {
                        Some(cap) => self.visited.len() < cap,
                        None => true,
                    };
                    if !can_insert {
                        current_node
                            .borrow_mut()
                            .set_expansion(skill.name(), parent_sig, None);
                        continue;
                    }

                    // Create new child node
                    id += 1;
                    let next_id = id;
                    let next_node = Node::new(
                        Some(current_node.clone()),
                        next_id,
                        skill.name(),
                        next_state,
                        next_cost,
                    );
                    let next_node = new_node_ref(next_node);
                    next_node.borrow_mut().set_canon(canon.clone());
                    // Record creation for summary
                    self.all_nodes.push(next_node.clone());
                    self.visited.insert(canon, next_node.clone());
                    // Record parents adjacency for this new edge
                    let entry = self.parents.entry(next_id).or_insert_with(Vec::new);
                    entry.push((current_node.clone(), skill.name().to_string()));
                    // 5: Add/ensure transition
                    current_node.borrow_mut().ensure_next_contains(next_node.clone());
                    // Update expansion mapping for this parent state
                    current_node
                        .borrow_mut()
                        .set_expansion(skill.name(), parent_sig, Some(next_node.clone()));
                    // 6: add to todo (at most once per node)
                    if in_todo.insert(next_id) {
                        todo.push(next_node);
                    }
                }
            }

            // Sort todo by cost
            todo.sort_by(|a, b| {
                b.borrow()
                    .cost()
                    .partial_cmp(&a.borrow().cost())
                    .unwrap_or(std::cmp::Ordering::Equal)
            });

            // println!("todo list ids:");
            // for node in todo.iter() {
            //     println!(
            //         "  Node {} (cost {})",
            //         node.borrow().id(),
            //         node.borrow().cost()
            //     );
            // }
        }

        // After search, write node and edge summaries capturing dedup + multi-parent structure
        if let Some(folder) = log_folder {
            // Nodes summary (extended)
            let nodes_path = format!("{}/nodes_summary.csv", folder);
            if let Ok(mut file) = File::create(&nodes_path) {
                let _ = writeln!(file, "id,parent_id,skill,cost,is_local_solution,is_updated,num_parents,canonical_state");
                for nref in &self.all_nodes {
                    let n = nref.borrow();
                    let id = n.id();
                    let parent_id = n.previous().as_ref().map(|p| p.borrow().id()).unwrap_or(0);
                    let skill = n.skill().unwrap_or("initial");
                    let cost = n.cost();
                    let is_local_solution = n.is_local_solution();
                    let is_updated = n.is_updated();
                    let num_parents = self.parents.get(&id).map(|v| v.len()).unwrap_or(0);
                    let canon = n.canon();
                    let _ = writeln!(
                        file,
                        "{},{},{},{},{},{},{},{}",
                        id, parent_id, skill, cost, is_local_solution, is_updated, num_parents, canon
                    );
                }
            }
            // Edges summary (multi-parent, including reused edges)
            let edges_path = format!("{}/edges_summary.csv", folder);
            if let Ok(mut file) = File::create(&edges_path) {
                let _ = writeln!(file, "parent_id,child_id,skill,reused,child_cost,child_updated_times");
                for (child_id, plist) in self.parents.iter() {
                    for (parent_ref, skill_name) in plist.iter() {
                        let parent_id = parent_ref.borrow().id();
                        let child_node = self
                            .all_nodes
                            .iter()
                            .find(|nr| nr.borrow().id() == *child_id)
                            .map(|nr| nr.borrow());
                        if let Some(cn) = child_node {
                            let reused = cn.previous().as_ref().map(|p| p.borrow().id() != parent_id).unwrap_or(true);
                            let child_cost = cn.cost();
                            let updated_times = cn.updated_times();
                            let _ = writeln!(
                                file,
                                "{},{},{},{},{:.3},{}",
                                parent_id, child_id, skill_name, reused, child_cost, updated_times
                            );
                        }
                    }
                }
            }
        }

        // Store elapsed; recap printing is deferred to Display (printed after Backward Solution)
        let elapsed = start_time.elapsed();
        self.elapsed = Some(elapsed);
    }

    pub fn all_nodes(&self) -> Vec<NodeRef> {
        // Prefer reporting all created nodes if available, to include pruned branches
        if !self.all_nodes.is_empty() {
            return self.all_nodes.clone();
        }
        // Fallback: traverse solution graph
        let mut nodes = Vec::new();
        if let Some(root) = &self.root {
            let mut seen: HashSet<usize> = HashSet::new();
            let mut todo = Vec::new();
            let root_id = root.borrow().id();
            if seen.insert(root_id) {
                nodes.push(root.clone());
                todo.push(root.clone());
            }
            while !todo.is_empty() {
                let current = todo.pop().unwrap();
                for n in current.borrow().solution_nodes().iter() {
                    let id = n.borrow().id();
                    if seen.insert(id) {
                        nodes.push(n.clone());
                        todo.push(n.clone());
                    }
                }
            }
        }
        nodes
    }

    /// Collects all recovery paths: (region_smt, enablement_path, cost).
    /// Only includes regions that are SAT under the model's system property.
    /// Capped to prevent runaway when solution graph has cycles.
    pub fn recovery_paths(&self, log_folder: &Option<&str>) -> Vec<(String, Vec<String>, f64)> {
        const MAX_RECOVERY_PATHS: usize = 10_000;
        let mut out: Vec<(String, Vec<String>, f64)> = Vec::new();
        let root = match &self.root {
            Some(r) => r,
            None => return out,
        };
        let root_borrow = root.borrow();
        'outer: for t in root_borrow.solutions().iter() {
            if out.len() >= MAX_RECOVERY_PATHS {
                break;
            }
            let region_smt = t.solved().clone();
            if !check_state(
                self.model,
                &region_smt,
                log_folder,
                Some("recovery_path_region"),
            ) {
                continue;
            }
            let mut stack: Vec<(NodeRef, Vec<String>)> = Vec::new();
            stack.push((t.next(), vec![t.skill().to_string()]));
            while let Some((node, path)) = stack.pop() {
                if out.len() >= MAX_RECOVERY_PATHS {
                    break 'outer;
                }
                let nb = node.borrow();
                let nexts = nb.solutions();
                if nexts.is_empty() {
                    out.push((region_smt.clone(), path, nb.cost()));
                } else {
                    for st in nexts.iter() {
                        let mut np = path.clone();
                        np.push(st.skill().to_string());
                        stack.push((st.next(), np));
                    }
                }
            }
        }
        out.sort_by(|a, b| a.2.partial_cmp(&b.2).unwrap_or(std::cmp::Ordering::Equal));
        out
    }
}
