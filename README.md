Ben Towers
V00978554
Simple Shell Interpreter (SSI)
Overview
This project implements a simple shell interpreter (SSI) in C. It supports basic shell functionalities, including executing commands, changing directories, and managing background jobs.

Features
Basic Functions:
Prompts for user input in the format: username@hostname: /home/user >
Executes commands like ls, cd, and pwd.

Foreground Execution:
Executes external programs.
Responds to Ctrl-C to stop long-running programs.
Outputs an error message for non-existent commands.

Changing Directories:
Supports changing to home (cd or cd ~).
Handles absolute and relative paths.

Background Execution:
Supports running commands in the background using bg.
Allows checking background jobs with bglist.

Compilation
To compile the project, run:

make
This will produce an executable named ssi.

Usage
To run the shell interpreter, execute:

./ssi or make run
The shell will await user commands.

Clean Up
To remove the compiled executable, use:

make clean

Known Limitations
The interpreter may not handle all edge cases for command execution and error reporting.