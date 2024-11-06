#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "job_list.h"

Job *job_list = NULL;  // Global pointer to the head of the job list

// Adds a new job to the job list with the specified PID and command arguments
void add_job(pid_t pid, char **args) {
    Job *new_job = (Job *)malloc(sizeof(Job)); // Allocate memory for a new job
    if (new_job == NULL) {
        perror("Failed to allocate memory for new job"); // Error handling for memory allocation failure
        return;
    }

    new_job->pid = pid;                           // Set the job's PID
    strcpy(new_job->command, args[0]);           // Copy the command into the job structure
    new_job->next = job_list;                     // Link the new job to the existing list
    job_list = new_job;                           // Update the head of the list to the new job
}

// Removes a job from the job list by its PID
void remove_job(pid_t pid) {
    Job *current = job_list;  // Pointer to traverse the job list
    Job *previous = NULL;      // Pointer to track the previous job

    // Traverse the job list to find the job with the specified PID
    while (current != NULL) {
        if (current->pid == pid) {  // Job found
            if (previous == NULL) {  // If it's the first job in the list
                job_list = current->next;  // Update head to the next job
            } else {
                previous->next = current->next;  // Bypass the current job
            }
            free(current);  // Free memory for the removed job
            return;  // Exit the function after removal
        }
        previous = current;  // Move to the next job
        current = current->next;
    }
}

// Lists all currently running background jobs in the job list
void bglist() {
    Job *current = job_list;  // Pointer to traverse the job list
    while (current != NULL) {
        printf("%d: %s\n", current->pid, current->command);  // Print the job's PID and command
        current = current->next;  // Move to the next job
    }
}

// Checks for terminated background jobs and removes them from the job list
void check_background_jobs() {
    pid_t terminated_pid;  // Variable to store the PID of a terminated job
    int status;  // Variable to store the termination status

    // Wait for any terminated jobs without blocking
    while ((terminated_pid = waitpid(-1, &status, WNOHANG)) > 0) {
        printf("%d: Job has terminated.\n", terminated_pid);  // Notify that the job has terminated
        remove_job(terminated_pid);  // Remove the terminated job from the job list
    }
}
