#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include "job_list.h"    

// Signal handler for SIGINT (Ctrl+C)
void handle_sigint(int sig) {
}

// Parses user input into individual arguments
void input_parser(char *input, char **args, int *arg_count) {
    char *token;  
    int index = 0;  
    token = strtok(input, " ");  // Tokenize input by spaces
    while (token != NULL) {  // Loop until all tokens are processed
        args[index] = token;  
        index++;
        token = strtok(NULL, " ");  
    }
    args[index] = NULL;  // Null-terminate the arguments array
    *arg_count = index;  // Set the count of arguments
}

// Handles the cd built-in command for changing directories
void change_directory(char **args, int arg_count) {
    // Check if the number of arguments is valid
    if (arg_count > 2) {
        printf("Error: too many arguments\n");
        return;
    } 
    // Change to home directory or specified path
    if (arg_count == 1 || strcmp(args[1], "~") == 0) {
        char *home = getenv("HOME");  // Get home directory path
        if (home == NULL) {
            perror("could not get HOME directory");
            return;
        }
        if (chdir(home) != 0) {  // Change directory to home
            perror("directory could not be changed to home");
        }
    } else {
        // Change to specified directory
        if (chdir(args[1]) != 0) {
            perror("directory could not be found");
        }
    }
}

// Handles the pwd built-in command for printing the current directory
void present_working_directory(char **args, int arg_count) {
    // Check if any arguments are provided
    if (arg_count > 1) {
        printf("Error: pwd does not take any arguments.\n");
    } else {
        char cwd[1024];  
        if (getcwd(cwd, sizeof(cwd)) != NULL) {  // Get current directory
           printf("%s\n", cwd);  
        } else {
           perror("pwd error");
        }
    }
}

// Executes external commands, handling both foreground and background processes
void execute_command(char **args, int arg_count) {
    // Check if the command is bg
    int bg_command = (strcmp(args[0], "bg") == 0); 
    if (bg_command && arg_count < 2) {
        printf("Error: bg requires a command to run\n");
        return;
    }

    pid_t pid = fork();  // Create a new process
    if (pid < 0) {  // Error in fork
        perror("Fork failed");
        return;
    } else if (pid == 0) {  // Child process
        if (bg_command) {  // If background 
            // Redirect output and error to /dev/null
            freopen("/dev/null", "w", stdout);
            freopen("/dev/null", "w", stderr);
            execvp(args[1], &args[1]);
            printf("%s: No such file or directory\n", args[1]);
        } else {  // If foreground 
            execvp(args[0], args);
            printf("%s: No such file or directory\n", args[0]);
        }
        exit(1);  // Exit child process on failure
    } else {  // Parent process
        if (bg_command) {  // If background 
            add_job(pid, &args[1]);  // Add job to the job list
            printf("Background job started with PID: %d\n", pid);
        } else {  // If foreground
            int status;
            signal(SIGINT, handle_sigint);  // Set signal handler for Ctrl+C
            waitpid(pid, &status, 0);  // Wait for child process to finish
        }
    }
}

int main() {
    char input[1024];  // Buffer for user input
    char *args[64];  // Array to hold parsed arguments
    char cwd[1024];  // Buffer for current working directory
    char hostname[1024];  // Buffer for hostname
    int arg_count = 0;  // Argument count
    char *username = getlogin();  // Get username of the logged-in user
    gethostname(hostname, sizeof(hostname));  // Get hostname of the machine  

    while (1) {  // Infinite loop for shell execution
        check_background_jobs();  // Check and update background jobs
        if (getcwd(cwd, sizeof(cwd)) == NULL) {  // Get current working directory
            perror("getcwd() error");
            return 1;
        }
        // Print shell prompt in the format username@hostname:current_directory >
        printf("%s@%s: %s > ", username, hostname, cwd);
        if (fgets(input, sizeof(input), stdin) == NULL) {  // Read user input
            break; 
        }
        input[strcspn(input, "\n")] = 0;  // Remove newline character
        if (strcmp(input, "exit") == 0) {  // Check for exit command
            break; 
        }
        input_parser(input, args, &arg_count);  // Parse input into arguments
        // Handle built-in commands
        if (strcmp(args[0], "cd") == 0) {
            change_directory(args, arg_count); 
        } else if (strcmp(args[0], "pwd") == 0) {
            present_working_directory(args, arg_count);
        } else if (strcmp(args[0], "bglist") == 0) {
            bglist();  // List background jobs
        } else {
            execute_command(args, arg_count);  // Execute external command
        }
    }
    return 0; 
}