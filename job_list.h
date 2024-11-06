#ifndef JOB_LIST_H
#define JOB_LIST_H

#include <sys/types.h>

// Structure to represent a background job
typedef struct Job {
    pid_t pid;               // Process ID of the job
    char command[256];      // Command associated with the job
    struct Job *next;       // Pointer to the next job in the list
} Job;

// Function prototypes for job management
void add_job(pid_t pid, char **args);  // Adds a new job to the job list
void remove_job(pid_t pid);             // Removes a job from the job list using its PID
void bglist();                          // Lists all currently running background jobs
void check_background_jobs();            // Checks the status of background jobs and updates the list

#endif 
