use crate::solver::*;
use std::collections::HashMap;

impl std::fmt::Display for Solver<'_> {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        // Print recap at the very end of execution, replacing the former summary
        writeln!(f, "Target skill: {}", self.display_skill().name())?;
        writeln!(f, "Max cost: {:.3}", self.display_max_cost())?;

        // Use recovery_paths, which already handles cycles and sorts by cost.
        // We then group by region_smt so we show at most one example per partition.
        let log_opt = self.display_last_log_folder();
        let paths = self.recovery_paths(&log_opt);

        if paths.is_empty() {
            writeln!(f, "Recovery paths found: none")?;
        } else {
            // Group by region_smt and keep the cheapest path for each region.
            let mut best: HashMap<String, (Vec<String>, f64)> = HashMap::new();
            for (region_smt, path, cost) in paths {
                best.entry(region_smt.clone())
                    .and_modify(|(p, c)| {
                        if cost < *c {
                            *p = path.clone();
                            *c = cost;
                        }
                    })
                    .or_insert((path, cost));
            }

            // Collect and sort regions by cost.
            let mut entries: Vec<(String, Vec<String>, f64)> = best
                .into_iter()
                .map(|(region, (path, cost))| (region, path, cost))
                .collect();
            entries.sort_by(|a, b| a.2.partial_cmp(&b.2).unwrap_or(std::cmp::Ordering::Equal));

            writeln!(f, "Recovery paths found (one example per partition):")?;
            const MAX_DISPLAY_PARTITIONS: usize = 100;
            for (idx, (region_smt, skills, cost)) in entries.iter().enumerate().take(MAX_DISPLAY_PARTITIONS) {
                let cond = region_smt;

                // Try to enumerate concrete state spaces for user-friendly display,
                // as in the original Display() implementation.
                let states = enumerate_state_spaces(
                    self.display_model(),
                    cond,
                    &log_opt,
                    Some(&format!("recap_partition_{}_enum", idx + 1)),
                    self.display_enum_cap(),
                );

                let rendered_cond = if !states.is_empty() {
                    if states.len() == 1 {
                        let one = &states[0];
                        let parts: Vec<String> = one
                            .iter()
                            .map(|(r, v)| format!("{}.{}", r, v))
                            .collect();
                        format!("{{{}}}", parts.join(", "))
                    } else {
                        let rendered: Vec<String> = states
                            .iter()
                            .map(|asg| {
                                let parts: Vec<String> = asg
                                    .iter()
                                    .map(|(r, v)| format!("{}.{}", r, v))
                                    .collect();
                                format!("{{{}}}", parts.join(", "))
                            })
                            .collect();
                        format!(
                            "{} (showing up to {})",
                            rendered.join(" OR "),
                            self.display_enum_cap()
                        )
                    }
                } else {
                    // Fallback to simplified boolean condition
                    pretty_condition(self.display_model(), cond, &log_opt)
                };

                let seq = skills.join(" → ");
                writeln!(
                    f,
                    "  {}. if {} then {} (cost {:.3})",
                    idx + 1,
                    rendered_cond,
                    seq,
                    cost
                )?;
            }
            if entries.len() > MAX_DISPLAY_PARTITIONS {
                writeln!(
                    f,
                    "  ... ({} more partition(s) omitted)",
                    entries.len() - MAX_DISPLAY_PARTITIONS
                )?;
            }
        }

        let secs = self
            .display_elapsed()
            .map(|d| d.as_secs_f64())
            .unwrap_or(0.0);
        writeln!(f, "Total runtime: {:.3}s", secs)?;

        Ok(())
    }
}
