use std::collections::{HashMap, VecDeque};
use std::env;
use std::fs::{self, OpenOptions, File};
use std::io::{self, Write, BufRead, BufReader, BufWriter};
use std::process::Command;
use termion::event::Event;
use termion::event::Key;
use termion::input::TermRead;
use termion::raw::IntoRawMode;

const HISTORY_LIMIT: usize = 1000;
const TRIM_PERCENTAGE: f32 = 0.75;

fn main() {
    let mut aliases: HashMap<String, String> = HashMap::new();
    let mut history: VecDeque<String> = load_history();
    let mut history_index: isize = -1;
    let home_dir = env::var("HOME").unwrap();
    let history_file_path = format!("{}/exo_bin/.exo_history", home_dir);

    // Set up custom aliases
    aliases.insert("ls".to_string(), format!("{}/exo_bin/exo_ls", home_dir));
    aliases.insert("cat".to_string(), format!("{}/exo_bin/exo_cat", home_dir));
    aliases.insert("echo".to_string(), format!("{}/exo_bin/exo_echo", home_dir));
    aliases.insert("pwd".to_string(), format!("{}/exo_bin/exo_pwd", home_dir));

    let stdin = io::stdin();
    let mut stdout = io::stdout().into_raw_mode().unwrap();
    let mut stdin_events = stdin.events();

    loop {
        // Display prompt
        print!("exo-shell$ ");
        stdout.flush().unwrap();

        let mut input = String::new();
        let mut current_input = String::new();

        for evt in stdin_events.by_ref() {
            match evt.unwrap() {
                Event::Key(Key::Char('\n')) => {
                    input = current_input.trim().to_string();
                    print!("\r\n");
                    stdout.flush().unwrap();

                    if !input.is_empty() {
                        save_command(&input, &mut history, &history_file_path);
                        history_index = -1;
                    }
                    break; // Break the for loop to process the input
                }
                Event::Key(Key::Char(c)) => {
                    if c == '\t' {
                        if let Some(suggestion) = autocomplete(&current_input, &aliases) {
                            for _ in 0..current_input.len() {
                                print!("\x08 \x08");
                            }
                            stdout.flush().unwrap();
                            current_input = suggestion;
                            print!("{}", current_input);
                            stdout.flush().unwrap();
                        }
                    } else {
                        current_input.push(c);
                        print!("{}", c);
                        stdout.flush().unwrap();
                    }
                }
                Event::Key(Key::Backspace) => {
                    if !current_input.is_empty() {
                        current_input.pop();
                        print!("\x08 \x08");
                        stdout.flush().unwrap();
                    }
                }
                Event::Key(Key::Ctrl('c')) => {
                    println!("\nExiting...");
                    return;
                }
                Event::Key(Key::Up) => {
                    if history_index + 1 < history.len() as isize {
                        history_index += 1;
                        current_input = history[history_index as usize].clone();
                        clear_line(&mut stdout, current_input.clone());
                    }
                }
                Event::Key(Key::Down) => {
                    if history_index > 0 {
                        history_index -= 1;
                        current_input = history[history_index as usize].clone();
                    } else {
                        history_index = -1;
                        current_input.clear();
                    }
                    clear_line(&mut stdout, current_input.clone());
                }
                _ => {}
            }
        }

        // Check for exit command after processing input
        if input == "exit" {
            break;
        }

        let parts: Vec<&str> = input.split_whitespace().collect();
        let command = parts.get(0).unwrap_or(&"");

        if *command == "cd" {
            if let Some(dir) = parts.get(1) {
                if let Err(e) = env::set_current_dir(dir) {
                    eprintln!("Error: {}", e);
                }
            } else {
                eprintln!("Usage: cd <directory>");
            }
            continue;
        }

        if let Some(program) = aliases.get(*command) {
            let args = &parts[1..];
            match Command::new(program).args(args).status() {
                Ok(status) if status.success() => (),
                Ok(_) => eprintln!("Error: Command failed to execute."),
                Err(e) => eprintln!("Error: Failed to run command '{}'. Reason: {}", program, e),
            }
        } else {
            eprintln!("Unknown command: {}", command);
        }
    }
}

/// Load history from `.exo_history` file in reverse order (newest first)
fn load_history() -> VecDeque<String> {
    let home_dir = env::var("HOME").unwrap();
    let history_file_path = format!("{}/exo_bin/.exo_history", home_dir);

    let mut history = VecDeque::new();
    if let Ok(file) = fs::File::open(&history_file_path) {
        let reader = BufReader::new(file);
        let lines: Vec<String> = reader.lines().filter_map(Result::ok).collect();
        for line in lines.into_iter().rev() {
            history.push_back(line);
        }
    }
    history
}

/// Save a command to the history file and memory
fn save_command(command: &str, history: &mut VecDeque<String>, history_file_path: &str) {
    // Enforce history limit in memory
    if history.len() >= HISTORY_LIMIT {
        // Calculate the number of oldest commands to remove
        let trim_count = (HISTORY_LIMIT as f32 * TRIM_PERCENTAGE).round() as usize;
        
        // Remove the oldest `trim_count` commands from memory
        for _ in 0..trim_count {
            history.pop_back();
        }

        // Rewrite the file with the trimmed history
        let file = File::create(history_file_path).unwrap();
        let mut writer = BufWriter::new(file);

        // Write the remaining commands to the file, oldest to newest
        for cmd in history.iter().rev() {
            writeln!(writer, "{}", cmd).unwrap();
        }
    }

    // Add the new command to the front of memory
    history.push_front(command.to_string());

    // Append the new command to the file
    let mut file = OpenOptions::new()
        .create(true)
        .append(true)
        .open(history_file_path)
        .unwrap();
    writeln!(file, "{}", command).unwrap();
}

fn autocomplete(current_input: &str, aliases: &HashMap<String, String>) -> Option<String> {
    for alias in aliases.keys() {
        if alias.starts_with(current_input) {
            return Some(alias.clone());
        }
    }
    let current_dir = env::current_dir().unwrap();
    if let Ok(entries) = fs::read_dir(current_dir) {
        for entry in entries.filter_map(Result::ok) {
            let filename = entry.file_name();
            let filename_str = filename.to_string_lossy();
            if filename_str.starts_with(current_input) {
                return Some(filename_str.to_string());
            }
        }
    }
    None
}

fn clear_line<W: Write>(stdout: &mut W, line: String) {
    print!("\r\x1b[Kexo-shell$ {}", line);
    stdout.flush().unwrap();
}
