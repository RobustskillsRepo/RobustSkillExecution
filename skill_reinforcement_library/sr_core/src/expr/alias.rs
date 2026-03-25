use crate::expr::BoolExpr;
use crate::model::ResourceValue;

//-------------------- From --------------------

impl From<bool> for BoolExpr {
    fn from(value: bool) -> Self {
        BoolExpr::Value(value)
    }
}

impl From<ResourceValue> for BoolExpr {
    fn from(rv: ResourceValue) -> Self {
        BoolExpr::Resource(Box::new(rv))
    }
}

impl From<&ResourceValue> for BoolExpr {
    fn from(rv: &ResourceValue) -> Self {
        BoolExpr::Resource(Box::new(rv.clone()))
    }
}

//-------------------- Iff --------------------

pub trait IffTrait<Rhs>
where
    Rhs: Into<BoolExpr>,
{
    fn iff(self, rhs: Rhs) -> BoolExpr;
}

impl IffTrait<BoolExpr> for BoolExpr {
    fn iff(self, rhs: BoolExpr) -> BoolExpr {
        BoolExpr::Iff(Box::new(self), Box::new(rhs))
    }
}

impl<Rhs> IffTrait<Rhs> for &BoolExpr
where
    Rhs: Into<BoolExpr>,
{
    fn iff(self, rhs: Rhs) -> BoolExpr {
        BoolExpr::Iff(Box::new(self.clone()), Box::new(rhs.into()))
    }
}

//-------------------- Implies --------------------

pub trait ImpliesTrait<Rhs>
where
    Rhs: Into<BoolExpr>,
{
    fn implies(self, rhs: Rhs) -> BoolExpr;
}

impl ImpliesTrait<BoolExpr> for BoolExpr {
    fn implies(self, rhs: BoolExpr) -> BoolExpr {
        BoolExpr::Implies(Box::new(self), Box::new(rhs))
    }
}

impl<Rhs> ImpliesTrait<Rhs> for &BoolExpr
where
    Rhs: Into<BoolExpr>,
{
    fn implies(self, rhs: Rhs) -> BoolExpr {
        BoolExpr::Implies(Box::new(self.clone()), Box::new(rhs.into()))
    }
}

//-------------------- Not --------------------

impl std::ops::Not for BoolExpr {
    type Output = BoolExpr;

    fn not(self) -> Self::Output {
        BoolExpr::Not(Box::new(self))
    }
}

impl std::ops::Not for &BoolExpr {
    type Output = BoolExpr;

    fn not(self) -> Self::Output {
        BoolExpr::Not(Box::new(self.clone()))
    }
}

//-------------------- And --------------------

impl std::ops::BitAnd for BoolExpr {
    type Output = BoolExpr;

    fn bitand(self, rhs: Self) -> Self::Output {
        BoolExpr::And(Box::new(self), Box::new(rhs))
    }
}

impl std::ops::BitAnd for &BoolExpr {
    type Output = BoolExpr;

    fn bitand(self, rhs: Self) -> Self::Output {
        BoolExpr::And(Box::new(self.clone()), Box::new(rhs.clone()))
    }
}

//-------------------- Or --------------------

impl std::ops::BitOr for BoolExpr {
    type Output = BoolExpr;

    fn bitor(self, rhs: Self) -> Self::Output {
        BoolExpr::Or(Box::new(self), Box::new(rhs))
    }
}

impl std::ops::BitOr for &BoolExpr {
    type Output = BoolExpr;

    fn bitor(self, rhs: Self) -> Self::Output {
        BoolExpr::Or(Box::new(self.clone()), Box::new(rhs.clone()))
    }
}
