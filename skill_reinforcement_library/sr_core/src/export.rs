//! Export of robust skill models to JSON per the Skill Execution Manager spec.
//! See robust_skills_ws/docs/skill_execution_manager_spec.md §3.2.

use crate::model::Model;
use crate::solver::{Solver, Verbosity};
use crate::solver::{check_state, ToSmt};
use serde::Serialize;
use std::collections::HashMap;
use std::fs;
use std::path::Path;

const ROBUST_MODEL_VERSION: &str = "1.0";

#[derive(Debug, Serialize)]
pub struct RobustModelResource {
    pub name: String,
    pub values: Vec<String>,
}

#[derive(Debug, Serialize)]
pub struct RecoverableEntry {
    pub region_smt: String,
    pub enablement_path: Vec<String>,
    pub cost: f64,
}

#[derive(Debug, Serialize)]
pub struct SkillPartition {
    pub nominal_smt: String,
    pub recoverable: Vec<RecoverableEntry>,
    pub unrecoverable_smt: String,
}

#[derive(Debug, Serialize)]
pub struct RobustModel {
    pub version: String,
    pub resources: Vec<RobustModelResource>,
    pub system_property_smt: String,
    pub skills: HashMap<String, SkillPartition>,
}

/// Options for robustifying a skillset.
#[derive(Clone, Debug)]
pub struct RobustifyOptions {
    pub max_cost: f64,
    pub enum_cap: usize,
    pub verbosity: Verbosity,
    /// If Some, write per-skill logs under this folder (e.g. "log/run_123").
    pub log_folder: Option<String>,
    /// Max distinct states (nodes) to explore per skill; prevents blow-up when SMT canonical forms vary. Default 2048.
    pub visited_cap: usize,
}

impl Default for RobustifyOptions {
    fn default() -> Self {
        Self {
            max_cost: 3.0,
            enum_cap: 5,
            verbosity: Verbosity::Quiet,
            log_folder: None,
            visited_cap: 2048,
        }
    }
}

/// Robustifies the entire skillset: for each skill runs the backward solver
/// treating it as the target skill, collects nominal/recoverable/
/// unrecoverable/impossible and enablement paths, then writes the robust model
/// to `robust_models/<model_name>.json`.
///
/// Transparent skills (pre == post) are still robustified as targets; the
/// `is_transparent` flag is only used *inside* the solver to avoid exploring
/// them as intermediate steps in recovery paths.
pub fn robustify_skillset(
    model: &Model,
    model_name: &str,
    options: &RobustifyOptions,
) -> Result<std::path::PathBuf, Box<dyn std::error::Error>> {
    let robust_models_dir = Path::new("robust_models");
    fs::create_dir_all(robust_models_dir)?;
    let output_path = robust_models_dir.join(format!("{}.json", model_name));

    let system_property_smt = model.system_property().to_smt();
    let resources: Vec<RobustModelResource> = model
        .resources()
        .iter()
        .map(|r| RobustModelResource {
            name: r.name().to_string(),
            values: r.values().to_vec(),
        })
        .collect();

    let mut skills: HashMap<String, SkillPartition> = HashMap::new();
    let total_skills = model.skills().len();

    println!(
        "Robustifying model '{}' ({} skills, max_cost = {}, enum_cap = {})",
        model_name, total_skills, options.max_cost, options.enum_cap
    );

    for (idx, skill) in model.skills().iter().enumerate() {
        println!(
            "[{}/{}] Skill '{}' (transparent = {})",
            idx + 1,
            total_skills,
            skill.name(),
            skill.is_transparent()
        );
        let nominal_smt = skill.precondition().to_smt();

        let log_folder: Option<String> = options
            .log_folder
            .as_ref()
            .map(|base| format!("{}/skill_{}", base, skill.name()));
        let log_ref = log_folder.as_deref();

        // Optionally check whether the nominal region is satisfiable under the system property.
        // Even if it is empty, we still run the backward solver to let it handle skills
        // without recovery paths.
        let _has_nominal = check_state(model, &nominal_smt, &None, Some("check_nominal"));

        println!(
            "  Running backward solver for '{}' (log_folder = {}).",
            skill.name(),
            log_folder.as_deref().unwrap_or("none")
        );

        let mut solver = Solver::new(model, skill, options.max_cost);
        solver.set_verbosity(options.verbosity);
        solver.set_enum_cap(options.enum_cap);
        solver.set_visited_cap(options.visited_cap);
        solver.solve(&log_ref);

        // When verbosity is Debug (-vv), also print the solver recap, which
        // uses enumerate_state_spaces to show concrete regions.
        if matches!(options.verbosity, Verbosity::Debug) {
            println!("\n===== Backward recap for target '{}' =====", skill.name());
            println!("{}", solver);
        }

        let paths = solver.recovery_paths(&log_ref);

        println!(
            "  Backward solver for '{}' produced {} recovery path(s).",
            skill.name(),
            paths.len()
        );

        let recoverable: Vec<RecoverableEntry> = paths
            .into_iter()
            .map(|(region_smt, enablement_path, cost)| RecoverableEntry {
                region_smt,
                enablement_path,
                cost,
            })
            .collect();

        println!(
            "  Exported {} recoverable entrie(s) for '{}'.",
            recoverable.len(),
            skill.name()
        );

        // Derive unrecoverable region formula:
        // S0 = ¬Pre_σ ∧ Φ_sys
        // Rec = ⋁ region_smt
        // Unrec = S0 ∧ ¬Rec
        let unrecoverable_smt = if recoverable.is_empty() {
            format!("(and (not {}) {})", nominal_smt, system_property_smt)
        } else {
            let mut rec_or = String::from("(or");
            for r in &recoverable {
                rec_or.push(' ');
                rec_or.push_str(&r.region_smt);
            }
            rec_or.push(')');
            format!(
                "(and (not {}) {} (not {}))",
                nominal_smt, system_property_smt, rec_or
            )
        };

        skills.insert(
            skill.name().to_string(),
            SkillPartition {
                nominal_smt,
                recoverable,
                unrecoverable_smt,
            },
        );
    }

    let robust = RobustModel {
        version: ROBUST_MODEL_VERSION.to_string(),
        resources,
        system_property_smt,
        skills,
    };

    let json = serde_json::to_string_pretty(&robust)?;
    fs::write(&output_path, json)?;
    Ok(output_path)
}
