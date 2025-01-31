# Simple Linux Shell

## Overview
This is a simple Linux shell implemented in C that supports basic command execution, piping, redirection, compound commands, and logical AND execution.

## Features
- **Basic command execution**: Runs standard Linux commands.
- **Change directory (`cd`)**: Built-in support for changing directories.
- **Piping (`|`)**: Supports command chaining using pipes.
- **Redirection (`>`, `>>`)**: Redirects output to files.
- **Compound commands (`;`)**: Allows multiple commands to be executed sequentially.
- **Logical AND (`&&`)**: Executes commands only if the previous command succeeds.
- **Prompt display**: Shows `[username@hostname:cwd]$` for an interactive experience.

## Compilation
To compile the shell, use the following command:

```sh
gcc -o shell shell.c
```

## Usage
Run the shell by executing:

```sh
./shell
```

### Examples
- Running a simple command:
  ```sh
  ls -l
  ```
- Changing directories:
  ```sh
  cd /home/user
  ```
- Using pipes:
  ```sh
  ls -l | grep "txt"
  ```
- Redirecting output to a file:
  ```sh
  ls > output.txt
  ```
- Appending output to a file:
  ```sh
  echo "Hello" >> output.txt
  ```
- Running multiple commands sequentially:
  ```sh
  echo "Hello"; echo "World"
  ```
- Using logical AND:
  ```sh
  mkdir test_dir && cd test_dir
  ```
- Exiting the shell:
  ```sh
  exit
  ```

## Limitations
- No support for background processes (`&`).
- Limited error handling.
- No support for advanced shell scripting.

## Future Enhancements
- Add support for background processes.
- Improve error handling and reporting.
- Implement environment variable expansion.
- Enhance the user interface with colors and autocomplete.

## License
This project is open-source and available under the MIT License.

