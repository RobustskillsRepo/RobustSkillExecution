use clap::{ArgAction, Parser, ValueEnum};
use sr::export::{robustify_skillset, RobustifyOptions};
use sr::models::quadruped::build_quadruped_model;
use sr::solver::{Solver, Verbosity};

#[derive(Copy, Clone, Debug, ValueEnum)]
enum ModelName {
    Quadruped,
}

#[derive(Parser, Debug)]
#[command(
    name = "skill_reinforcement",
    version,
    about = "Design-time robustification tools for skill models"
)]
struct Args {
    /// Which built-in model to use
    #[arg(long, value_enum, default_value_t = ModelName::Quadruped)]
    model: ModelName,

    /// If set, robustify only this skill and print the recap
    #[arg(long)]
    skill: Option<String>,

    /// Maximum number of concrete state spaces to list per path in recap
    #[arg(long, default_value_t = 5)]
    enum_cap: usize,

    /// Increase verbosity (-v for normal, -vv for debug)
    #[arg(short = 'v', long = "verbose", action = ArgAction::Count)]
    verbose: u8,

    /// Maximum cumulative cost allowed for recovery paths
    #[arg(long, default_value_t = 3.0)]
    max_cost: f64,

    /// Disable logging (on by default)
    #[arg(long = "no-log", action = ArgAction::SetTrue, default_value_t = false)]
    no_log: bool,

    /// Output model name (JSON written to robust_models/<name>.json) when robustifying the whole skillset
    #[arg(long, default_value = "quadruped_example")]
    output_model_name: String,
}

fn main() {
    let args = Args::parse();

    let verbosity = match args.verbose {
        0 => Verbosity::Quiet,
        1 => Verbosity::Normal,
        _ => Verbosity::Debug,
    };

    let model = match args.model {
        ModelName::Quadruped => build_quadruped_model(),
    };

    if let Some(skill_name) = args.skill.as_deref() {
        // Single-skill mode: print recap to terminal
        let skill = model
            .get_skill(skill_name)
            .unwrap_or_else(|| {
                eprintln!("Unknown skill '{}' for selected model.", skill_name);
                std::process::exit(1);
            });

        println!(
            "Robustifying single skill '{}' (model: {:?})",
            skill.name(),
            args.model
        );

        let log_folder: Option<String> = if args.no_log {
            None
        } else {
            use std::time::{SystemTime, UNIX_EPOCH};
            let now = SystemTime::now()
                .duration_since(UNIX_EPOCH)
                .unwrap_or_default();
            let pid = std::process::id();
            let run = format!("run-{}_{}-{}", now.as_secs(), now.subsec_nanos(), pid);
            Some(format!("log/{}", run))
        };
        let log_ref = log_folder.as_deref();

        let mut solver = Solver::new(&model, skill, args.max_cost);
        solver.set_verbosity(verbosity);
        solver.set_enum_cap(args.enum_cap);
        solver.set_visited_cap(2048);
        solver.solve(&log_ref);

        // Always print recap for single-skill mode
        println!("\n===== Backward recap for target '{}' =====", skill.name());
        println!("{}", solver);
    } else {
        // Whole-skillset mode: export JSON robust model
        let log_folder: Option<String> = if args.no_log {
            None
        } else {
            use std::time::{SystemTime, UNIX_EPOCH};
            let now = SystemTime::now()
                .duration_since(UNIX_EPOCH)
                .unwrap_or_default();
            let pid = std::process::id();
            let run = format!("run-{}_{}-{}", now.as_secs(), now.subsec_nanos(), pid);
            Some(format!("log/{}", run))
        };

        let options = RobustifyOptions {
            max_cost: args.max_cost,
            enum_cap: args.enum_cap,
            verbosity,
            log_folder: log_folder.clone(),
            visited_cap: 2048,
        };

        match robustify_skillset(&model, &args.output_model_name, &options) {
            Ok(path) => println!("Robust model written to {}", path.display()),
            Err(e) => {
                eprintln!("Failed to robustify skillset: {}", e);
                std::process::exit(1);
            }
        }
    }
}
