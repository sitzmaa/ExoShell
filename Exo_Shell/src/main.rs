use std::collections::HashMap;
use std::env;
use std::fs;
use std::io::{self, Write};
use std::process::Command;
use termion::event::Event;
use termion::event::Key;
use termion::input::TermRead;
use termion::raw::IntoRawMode;

fn main() {
    let mut aliases: HashMap<String, String> = HashMap::new();

    // Add custom command aliases
    aliases.insert("ls".to_string(), "./exo_ls".to_string());
    aliases.insert("cat".to_string(), "./exo_cat".to_string());
    aliases.insert("echo".to_string(), "./exo_echo".to_string());
    aliases.insert("pwd".to_string(), "./exo_pwd".to_string());

    let stdin = io::stdin(); // Keep a reference to stdin
    let mut stdout = io::stdout().into_raw_mode().unwrap();
    let mut stdin_events = stdin.events(); // Declare as mutable

    loop {
        // Display prompt
        print!("exo-shell$ ");
        stdout.flush().unwrap();

        let mut input = String::new();
        let mut current_input = String::new();

        for evt in stdin_events.by_ref() {
            match evt.unwrap() {
                Event::Key(Key::Char('\n')) => {
                    // If Enter is pressed, process the command
                    input = current_input.trim().to_string();

                    print!("\r\n");
                    stdout.flush().unwrap();
                    break;
                }
                Event::Key(Key::Char(c)) => {
                    // Check if the character is a tab
                    if c == '\t' {
                        // Perform tab autocompletion
                        if let Some(suggestion) = autocomplete(&current_input, &aliases) {
                            // Clear current input
                            for _ in 0..current_input.len() {
                                print!("\x08 \x08");
                            }
                            stdout.flush().unwrap();

                            // Fill in the suggested command
                            current_input = suggestion;
                            print!("{}", current_input);
                            stdout.flush().unwrap();
                        }
                    } else {
                        // Collect normal characters into the input
                        current_input.push(c);
                        print!("{}", c);
                        stdout.flush().unwrap();
                    }
                }
                Event::Key(Key::Backspace) => {
                    if !current_input.is_empty() {
                        current_input.pop();
                        print!("\x08 \x08"); // Handle backspace
                        stdout.flush().unwrap();
                    }
                }
                Event::Key(Key::Ctrl('c')) => {
                    // Exit on Ctrl+C
                    println!("\nExiting...");
                    return;
                }
                _ => {}
            }
        }

        // Exit shell
        if input == "exit" {
            break;
        }

        // Split input into command and arguments
        let parts: Vec<&str> = input.split_whitespace().collect();
        let command = parts.get(0).unwrap_or(&"");

        // Handle the built-in `cd` command
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

        // Check if the command is in the alias map
        if let Some(program) = aliases.get(*command) {
            // Build command with arguments
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

/// Autocompletion logic that tries to autocomplete commands or file paths
fn autocomplete(current_input: &str, aliases: &HashMap<String, String>) -> Option<String> {
    // Check if current input matches a known command or alias
    for alias in aliases.keys() {
        if alias.starts_with(current_input) {
            return Some(alias.clone());
        }
    }

    // Try to autocomplete file paths in the current directory
    let current_dir = env::current_dir().unwrap();
    if let Ok(entries) = fs::read_dir(current_dir) {
        for entry in entries {
            if let Ok(entry) = entry {
                let filename = entry.file_name();
                let filename_str = filename.to_string_lossy();
                if filename_str.starts_with(current_input) {
                    return Some(filename_str.to_string());
                }
            }
        }
    }

    None // No match found
}
