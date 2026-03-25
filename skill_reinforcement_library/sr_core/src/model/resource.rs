use crate::model::ResourceValue;

#[derive(Debug, Clone)]
pub struct Resource {
    name: String,
    values: Vec<String>,
}

impl Resource {
    pub fn empty<T: Into<String>>(name: T) -> Self {
        Self {
            name: name.into(),
            values: Vec::new(),
        }
    }

    pub fn new<T: Into<String>, U: Into<String>>(name: T, values: Vec<U>) -> Self {
        Self {
            name: name.into(),
            values: values.into_iter().map(Into::into).collect(),
        }
    }

    pub fn name(&self) -> &str {
        &self.name
    }

    pub fn values(&self) -> &[String] {
        &self.values
    }

    pub fn variables(&self) -> Vec<ResourceValue> {
        self.values
            .iter()
            .map(|v| ResourceValue::new(self.name.clone(), v.clone()))
            .collect()
    }

    pub fn add_value<T: Into<String>>(&mut self, value: T) -> ResourceValue {
        self.values.push(value.into());
        ResourceValue::new(self.name.clone(), self.values.last().unwrap().clone())
    }
}

impl std::fmt::Display for Resource {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{}", self.name())
    }
}
