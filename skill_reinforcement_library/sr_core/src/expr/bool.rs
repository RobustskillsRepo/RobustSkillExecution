use crate::model::*;
use std::{collections::HashSet, hash::Hash};

#[derive(Debug, Clone, PartialEq, Eq, Hash)]
pub enum BoolExpr {
    Value(bool),
    Resource(Box<ResourceValue>),
    Not(Box<BoolExpr>),
    And(Box<BoolExpr>, Box<BoolExpr>),
    Or(Box<BoolExpr>, Box<BoolExpr>),
    Iff(Box<BoolExpr>, Box<BoolExpr>),
    Implies(Box<BoolExpr>, Box<BoolExpr>),
}

impl BoolExpr {
    pub fn resources(&self) -> HashSet<String> {
        match self {
            BoolExpr::Value(_) => HashSet::new(),
            BoolExpr::Resource(rv) => HashSet::from([rv.resource().to_string()]),
            BoolExpr::Not(expr) => expr.resources(),
            BoolExpr::And(lhs, rhs) => {
                let mut resources = lhs.resources();
                resources.extend(rhs.resources());
                resources
            }
            BoolExpr::Or(lhs, rhs) => {
                let mut resources = lhs.resources();
                resources.extend(rhs.resources());
                resources
            }
            BoolExpr::Iff(lhs, rhs) => {
                let mut resources = lhs.resources();
                resources.extend(rhs.resources());
                resources
            }
            BoolExpr::Implies(lhs, rhs) => {
                let mut resources = lhs.resources();
                resources.extend(rhs.resources());
                resources
            }
        }
    }

    pub fn unchanged_resources(&self, model: &Model) -> Vec<Resource> {
        let changed = self.resources();
        model
            .resources()
            .iter()
            .filter_map(|r| {
                if !changed.contains(r.name()) {
                    Some(r.clone())
                } else {
                    None
                }
            })
            .collect()
    }
}

impl std::fmt::Display for BoolExpr {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            BoolExpr::Value(v) => write!(f, "{}", v),
            BoolExpr::Resource(rv) => write!(f, "{}", rv),
            BoolExpr::Not(expr) => write!(f, "(¬ {})", expr),
            BoolExpr::And(lhs, rhs) => write!(f, "({} ∧ {})", lhs, rhs),
            BoolExpr::Or(lhs, rhs) => write!(f, "({} ∨ {})", lhs, rhs),
            BoolExpr::Iff(lhs, rhs) => write!(f, "({} ⇔ {})", lhs, rhs),
            BoolExpr::Implies(lhs, rhs) => write!(f, "({} ⇒ {})", lhs, rhs),
        }
    }
}
