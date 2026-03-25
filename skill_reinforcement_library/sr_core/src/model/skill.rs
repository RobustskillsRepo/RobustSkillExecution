use crate::expr::BoolExpr;

#[derive(Debug, Clone)]
pub struct Skill {
    name: String,
    precondition: BoolExpr,
    postcondition: BoolExpr,
    cost: f64,
}

impl Skill {
    pub fn new<T: Into<String>>(
        name: T,
        precondition: BoolExpr,
        postcondition: BoolExpr,
        cost: f64,
    ) -> Self {
        Self {
            name: name.into(),
            precondition,
            postcondition,
            cost,
        }
    }

    pub fn name(&self) -> &str {
        &self.name
    }

    pub fn precondition(&self) -> &BoolExpr {
        &self.precondition
    }

    pub fn postcondition(&self) -> &BoolExpr {
        &self.postcondition
    }

    pub fn cost(&self) -> f64 {
        self.cost
    }

    /// Returns true if this skill is transparent (pre == post, meaning it doesn't change state)
    pub fn is_transparent(&self) -> bool {
        self.precondition == self.postcondition
    }
}

impl std::fmt::Display for Skill {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{}", self.name(),)
    }
}
