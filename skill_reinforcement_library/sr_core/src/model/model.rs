use crate::expr::BoolExpr;
use crate::model::{Resource, Skill};

#[derive(Clone, Debug)]
pub struct Model {
    resources: Vec<Resource>,
    skills: Vec<Skill>,
    // Plant-wide constraints that are always true (e.g., physical impossibilities)
    system_property: BoolExpr,
}

impl Model {
    pub fn new(resources: Vec<Resource>, skills: Vec<Skill>) -> Self {
        Self {
            resources,
            skills,
            system_property: BoolExpr::from(true),
        }
    }

    pub fn resources(&self) -> &Vec<Resource> {
        &self.resources
    }

    pub fn add_resource(&mut self, resource: Resource) {
        self.resources.push(resource);
    }

    pub fn variables(&self) -> Vec<crate::model::ResourceValue> {
        self.resources.iter().flat_map(|r| r.variables()).collect()
    }

    pub fn skills(&self) -> &Vec<Skill> {
        &self.skills
    }

    pub fn get_skill(&self, name: &str) -> Option<&Skill> {
        self.skills.iter().find(|s| s.name() == name)
    }

    pub fn add_skill(&mut self, skill: Skill) {
        self.skills.push(skill);
    }

    // ---------------- System Property ---------------- //

    pub fn set_system_property(&mut self, prop: BoolExpr) {
        self.system_property = prop;
    }

    pub fn system_property(&self) -> &BoolExpr {
        &self.system_property
    }
}

impl Default for Model {
    fn default() -> Self {
        Self {
            resources: Vec::new(),
            skills: Vec::new(),
            system_property: BoolExpr::from(true),
        }
    }
}

impl std::fmt::Display for Model {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        writeln!(f, "Resources {{")?;
        for r in &self.resources {
            writeln!(f, "  {}: {{ {} }}", r.name(), r.values().join(", "))?;
        }
        writeln!(f, "}}")?;
        writeln!(f, "System Property:")?;
        writeln!(f, "  {}", self.system_property)?;
        writeln!(f, "Skills {{")?;
        for s in &self.skills {
            writeln!(f, "  {}: {{", s.name())?;
            writeln!(f, "    pre : {}", s.precondition())?;
            writeln!(f, "    post: {}", s.postcondition())?;
            writeln!(f, "    cost: {}", s.cost())?;
            writeln!(f, "  }}")?;
        }
        writeln!(f, "}}")
    }
}
