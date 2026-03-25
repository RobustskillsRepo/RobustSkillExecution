use crate::expr::BoolExpr;

#[derive(Debug, Clone, Hash, PartialEq, Eq)]
pub struct ResourceValue {
    resource: String,
    value: String,
}

impl ResourceValue {
    pub fn new<T: Into<String>, U: Into<String>>(resource: T, value: U) -> Self {
        Self {
            resource: resource.into(),
            value: value.into(),
        }
    }

    pub fn resource(&self) -> &str {
        &self.resource
    }

    pub fn value(&self) -> &str {
        &self.value
    }

    pub fn expr(&self) -> BoolExpr {
        BoolExpr::Resource(Box::new(self.clone()))
    }
}

impl std::fmt::Display for ResourceValue {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{}.{}", self.resource(), self.value())
    }
}
