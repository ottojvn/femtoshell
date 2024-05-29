#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define TRUE 1
#define BUFFER_SIZE 256

// Function to check if a given command is full path and executable
int is_executable(const char *path);

// Function to search for the command in directories specified by PATH
char *find_command_in_path(const char *command);

int main(void)
{
    char buffer[BUFFER_SIZE];

    while (TRUE)
    {
        // Prompt
        printf("$ ");
        fflush(stdout); // Ensure prompt is displayed immediately

        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL)
        {
            perror("fgets");
            continue;
        }

        // Remove newline character from buffer
        buffer[strcspn(buffer, "\n")] = 0;

        // Tokenize
        static char *newargv[BUFFER_SIZE];
        static char *newenv[] = {NULL};
        char *token = strtok(buffer, " ");
        int i = 0;
        while (token != NULL)
        {
            newargv[i++] = token;
            token = strtok(NULL, " ");
        }
        newargv[i] = NULL; // Null-terminate the argument list

        if (i == 0)
        {
            // No command entered
            continue;
        }

        // Find the command in PATH
        char *command_path = find_command_in_path(newargv[0]);
        if (command_path == NULL)
        {
            fprintf(stderr, "Command not found: %s\n", newargv[0]);
            continue;
        }

        // Fork and execve
        pid_t pid = fork();
        if (pid == -1)
        {
            perror("fork failure");
            free(command_path);
            continue;
        }
        else if (pid == 0)
        {
            // Child process
            execve(command_path, newargv, newenv);
            perror("execve failure"); // Only reached if execve fails
            exit(EXIT_FAILURE);       // Exit child process if execve fails
        }
        else
        {
            // Parent process
            free(command_path);
            int wstatus;
            do
            {
                if (waitpid(pid, &wstatus, 0) == -1)
                {
                    perror("waitpid");
                    break;
                }

                if (WIFEXITED(wstatus))
                {
                    continue;
                    // printf("exited, status=%d\n", WEXITSTATUS(wstatus));
                }
                else if (WIFSIGNALED(wstatus))
                {
                    printf("killed by signal %d\n", WTERMSIG(wstatus));
                }
                else if (WIFSTOPPED(wstatus))
                {
                    printf("stopped by signal %d\n", WSTOPSIG(wstatus));
                }
            } while (!WIFEXITED(wstatus) && !WIFSIGNALED(wstatus));
        }
    }

    return 0;
}

char *find_command_in_path(const char *command)
{
    if (is_executable(command))
    {
        return strdup(command);
    }

    // Get the PATH environment variable
    char *path_env = getenv("PATH");
    if (!path_env)
    {
        return NULL;
    }

    // Duplicate the PATH string because strtok modifies the string it tokenizes
    char *path_copy = strdup(path_env);
    if (!path_copy)
    {
        perror("strdup");
        return NULL;
    }

    // Tokenize the PATH variable to get individual directories
    char *dir = strtok(path_copy, ":");
    while (dir != NULL)
    {
        // Construct the full path to the command
        static char full_path[BUFFER_SIZE];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir, command);

        // Check if the command exists in this directory
        if (is_executable(full_path))
        {
            free(path_copy);
            return strdup(full_path);
        }

        // Get the next directory
        dir = strtok(NULL, ":");
    }

    free(path_copy);
    return NULL;
}

int is_executable(const char *path)
{
    return access(path, X_OK) == 0;
}
