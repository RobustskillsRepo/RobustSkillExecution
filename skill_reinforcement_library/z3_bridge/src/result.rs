#[derive(Debug, Clone, Copy, PartialEq, Hash)]
pub enum SatResult {
    Unknown,
    Unsat,
    Sat,
}

impl std::fmt::Display for SatResult {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            SatResult::Unknown => write!(f, "unknown"),
            SatResult::Unsat => write!(f, "unsat"),
            SatResult::Sat => write!(f, "sat"),
        }
    }
}
