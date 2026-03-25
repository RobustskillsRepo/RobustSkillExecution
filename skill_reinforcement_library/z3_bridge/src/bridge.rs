use crate::result::SatResult;
use std::fs::File;
use std::io::{BufRead, BufReader, BufWriter, Write};
use std::process::{Child, ChildStdin, ChildStdout, ExitStatus};
use std::process::{Command, Stdio};

pub struct Z3Bridge {
    child: Child,
    log_writer: Option<BufWriter<File>>,
    //
    in_writer: BufWriter<ChildStdin>,
    out_reader: BufReader<ChildStdout>,
    // err_reader: BufReader<ChildStderr>,
}

impl Z3Bridge {
    fn option_to_result<T>(opt: Option<T>, err_msg: &str) -> std::io::Result<T> {
        opt.ok_or_else(|| std::io::Error::new(std::io::ErrorKind::Other, err_msg))
    }

    pub fn new(program: &str, args: Vec<&str>, log_file: Option<String>) -> std::io::Result<Self> {
        let log_writer = match log_file {
            Some(file_name) => {
                let file = File::create(file_name)?;
                Some(BufWriter::new(file))
            }
            None => None,
        };

        let mut command = Command::new(program);
        command
            .args(args)
            .stdin(Stdio::piped())
            .stdout(Stdio::piped())
            // .stderr(Stdio::piped())
            ;

        let mut child = command.spawn()?;

        let stdin = Self::option_to_result(child.stdin.take(), "stdin error")?;
        let stdout = Self::option_to_result(child.stdout.take(), "stdout error")?;
        // let stderr = Self::option_to_result(child.stderr.take(), "stderr error")?;

        let in_writer = BufWriter::new(stdin);
        let out_reader = BufReader::new(stdout);
        // let err_reader = BufReader::new(stderr);

        Ok(Self {
            child,
            log_writer,
            in_writer,
            out_reader,
            // err_reader,
        })
    }

    fn write_log(&mut self, line: &str) -> std::io::Result<()> {
        if let Some(file) = &mut self.log_writer {
            write!(file, "{}\n", line)?;
            file.flush()?;
        }
        Ok(())
    }

    pub fn wait(&mut self) -> std::io::Result<ExitStatus> {
        self.child.wait()
    }

    pub fn write_line(&mut self, line: &str) -> std::io::Result<()> {
        write!(self.in_writer, "{}\n", line)?;
        self.write_log(line)
    }

    pub fn flush(&mut self) -> std::io::Result<()> {
        self.in_writer.flush()
    }

    pub fn read_line(&mut self) -> std::io::Result<String> {
        let mut line = String::new();
        match self.out_reader.read_line(&mut line) {
            Ok(_) => Ok(line.trim().to_string()),
            Err(e) => Err(e),
        }
    }

    //------------------------- SMT API -------------------------

    pub fn add_comment(&mut self, comment: &str) -> std::io::Result<()> {
        if self.log_writer.is_some() {
            for c in comment.split('\n') {
                let line = format!("; {}", c);
                self.write_line(&line)?;
            }
        }
        Ok(())
    }

    pub fn set_option(&mut self, option: &str, value: &str) -> std::io::Result<()> {
        let line = format!("(set-option :{} {})", option, value);
        self.write_line(&line)
    }

    pub fn declare_const_bool(&mut self, name: &str) -> std::io::Result<()> {
        let line = format!("(declare-const {} Bool)", name);
        self.write_line(&line)
    }

    pub fn declare_const_int(&mut self, name: &str) -> std::io::Result<()> {
        let line = format!("(declare-const {} Int)", name);
        self.write_line(&line)
    }

    pub fn declare_const_real(&mut self, name: &str) -> std::io::Result<()> {
        let line = format!("(declare-const {} Real)", name);
        self.write_line(&line)
    }

    pub fn assert(&mut self, expr: &str) -> std::io::Result<()> {
        let line = format!("(assert {})", expr);
        self.write_line(&line)
    }

    pub fn apply(&mut self, tactic: &str) -> std::io::Result<String> {
        let line = format!("(apply {})", tactic);
        self.write_line(&line)?;
        self.flush()?;

        let mut goals = "(and true".to_string();
        let mut count = 0;

        loop {
            let s = self.read_line()?;
            count += s.chars().filter(|&c| c == '(').count();
            count -= s.chars().filter(|&c| c == ')').count();
            if count == 0 {
                goals += "\n)";
                return Ok(goals);
            }
            if !s.is_empty()
                && !s.contains("goal")
                && !s.contains(":precision")
                && !s.contains(":depth")
            {
                goals += format!("\n\t{}", s.trim()).as_str();
            }
        }
    }

    pub fn check_sat(&mut self) -> std::io::Result<SatResult> {
        self.write_line("(check-sat)")?;
        self.flush()?;
        // loop {
        let response = self.read_line()?;
        match response.as_str() {
            "unknown" => return Ok(SatResult::Unknown),
            "unsat" => return Ok(SatResult::Unsat),
            "sat" => return Ok(SatResult::Sat),
            _ => panic!(), // }
        }
    }

    pub fn get_model(&mut self) -> std::io::Result<String> {
        let line = "(get-model)";
        self.write_line(&line)?;
        self.flush()?;

        let mut result = "".to_string();
        let mut count = 0;

        loop {
            let s = self.read_line()?;
            result += &format!("{}", s);

            count += s.chars().filter(|&c| c == '(').count();
            count -= s.chars().filter(|&c| c == ')').count();
            if count == 0 {
                return Ok(result);
            }
            result += "\n";
        }
    }

    pub fn eval(&mut self, expr: &str) -> std::io::Result<String> {
        let line = format!("(eval {})", expr);
        self.write_line(&line)?;
        self.flush()?;
        let result = self.read_line()?;
        Ok(result)
    }

    pub fn exit(&mut self) -> std::io::Result<()> {
        self.write_line("(exit)")?;
        self.flush()
    }
}

impl Default for Z3Bridge {
    fn default() -> Self {
        Self::new("z3", vec!["-in"], Some("log.smt".into())).unwrap()
    }
}
