use clap::{ArgAction, Parser};
use sr::export::{robustify_skillset, RobustifyOptions};
use sr::models::quadruped::build_quadruped_model;

#[derive(Parser, Debug)]
#[command(name = "quadruped_example", about = "Robustify entire quadruped skillset and export robust model JSON")]
struct Args {
    /// Maximum number of concrete state spaces to list per path in recap
    #[arg(long, default_value_t = 5)]
    enum_cap: usize,

    /// Increase verbosity (-v for normal, -vv for debug)
    #[arg(short = 'v', long = "verbose", action = ArgAction::Count)]
    verbose: u8,

    /// Maximum cumulative cost allowed for recovery paths per skill
    #[arg(long, default_value_t = 3.0)]
    max_cost: f64,

    /// Disable logging (on by default)
    #[arg(long = "no-log", action = ArgAction::SetTrue, default_value_t = false)]
    no_log: bool,

    /// Output model name (JSON written to robust_models/<name>.json)
    #[arg(long, default_value = "quadruped_example")]
    model_name: String,
}

fn main() {
    let args = Args::parse();
    let verbosity = match args.verbose {
        0 => sr::solver::Verbosity::Quiet,
        1 => sr::solver::Verbosity::Normal,
        _ => sr::solver::Verbosity::Debug,
    };
    let model = build_quadruped_model();

    if !matches!(verbosity, sr::solver::Verbosity::Quiet) {
        print!("{}", model);
    }

    let log_folder: Option<String> = if args.no_log {
        None
    } else {
        use std::time::{SystemTime, UNIX_EPOCH};
        let now = SystemTime::now().duration_since(UNIX_EPOCH).unwrap_or_default();
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

    match robustify_skillset(&model, &args.model_name, &options) {
        Ok(path) => println!("Robust model written to {}", path.display()),
        Err(e) => {
            eprintln!("Failed to robustify skillset: {}", e);
            std::process::exit(1);
        }
    }
}
