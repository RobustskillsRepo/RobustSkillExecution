// simplify, propagate-values, ctx-simplify, reduce-args

pub const PRE_SAT_TACTIC: &'static str = r#"
(repeat
    (then
    blast-term-ite        ; déniche les ITE profonds            
    simplify              ; réécriture algébrique
    propagate-values      ; remplace par les constantes connues 
    propagate-ineqs       ; propage les bornes arithmétiques
    ctx-simplify          ; simplification contextuelle
    )
2
)"#;

pub const PRE_QE_TACTIC: &'static str = r#"
(repeat
    (then
    simplify              ; réécriture algébrique
    propagate-values      ; remplace par les constantes connues
    propagate-ineqs       ; propage les bornes arithmétiques
    )
2
)"#;

pub const POST_QE_TACTIC: &'static str = r#"
(repeat
    (then
    simplify              ;    réécriture algébrique
    propagate-values      ; remplace par les constantes connues
    propagate-ineqs       ; propage les bornes arithmétiques
    ctx-simplify          ; simplification contextuelle
    )
2
)"#;

pub fn simplify_tactic() -> String {
    PRE_SAT_TACTIC.to_string()
}

pub fn qe_tactic() -> String {
    format!("(then {} qe-light qe {})", PRE_QE_TACTIC, POST_QE_TACTIC)
}
