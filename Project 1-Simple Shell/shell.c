#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

#define MAX_COMMAND_LINE_LEN 1024
#define MAX_COMMAND_LINE_ARGS 128

char prompt[] = "> ";
char delimiters[] = " \t\r\n";
extern char **environ;
int cmd_pid = -1;  // Stores the PID of the current foreground command

// Function declarations
void changeDir(char *arguments[]);
void getWorkingDir();
void getEnv(char *arguments[]);
void echoFunc(char *arguments[]);
void setEnv(char *arguments[]);
void exitShell();
void parseQuotes(char *token);
void cmd_exec(char *arguments[], bool background);
void executePipedCommands(char *commands[][MAX_COMMAND_LINE_ARGS], int num_commands);
void signal_handler(int signum);
void timeout_process(int time, int pid);
bool handleIO(char *arguments[], int *input_fd, int *output_fd);

int main() {
    // Stores the string typed into the command line.
    size_t size = 300;
    char cmd_dir[size];
    char command_line[MAX_COMMAND_LINE_LEN];

    // Stores the tokenized command line input.
    char *arguments[MAX_COMMAND_LINE_ARGS];
    signal(SIGINT, signal_handler);

    while (true) {
        // Get and print the current directory in the prompt
        fflush(stdout);
        char *cmd_dir_ptr = getcwd(cmd_dir, size);
        printf("%s%s", cmd_dir_ptr, prompt);
        fflush(stdout);

        // Read input from stdin and store it in command_line. Exit if error.
        if ((fgets(command_line, MAX_COMMAND_LINE_LEN, stdin) == NULL) && ferror(stdin)) {
            fprintf(stderr, "Error reading command line input.\n");
            exit(0);
        }

        // Check for EOF (ctrl+d)
        if (feof(stdin)) {
            printf("\n");
            fflush(stdout);
            return 0;
        }

        // Tokenize the command line input (split it on whitespace)
        arguments[0] = strtok(command_line, delimiters);
        int position = 0;

        while (arguments[position] != NULL) {
            parseQuotes(arguments[position]);
            position++;
            arguments[position] = strtok(NULL, delimiters);
        }

        // Skip empty input
        if (arguments[0] == NULL) continue;

        // Handle background processes
        bool background = false;
        if (strcmp(arguments[position-1], "&") == 0) {
            background = true;
            arguments[position-1] = NULL;
        }

        // Implement Built-In Commands
        if (strcmp(arguments[0], "cd") == 0) {
            changeDir(arguments);
        } else if (strcmp(arguments[0], "pwd") == 0) {
            getWorkingDir();
        } else if (strcmp(arguments[0], "env") == 0) {
            getEnv(arguments);
        } else if (strcmp(arguments[0], "echo") == 0) {
            echoFunc(arguments);
        } else if (strcmp(arguments[0], "setenv") == 0) {
            setEnv(arguments);
        } else if (strcmp(arguments[0], "exit") == 0) {
            exitShell();
        } else {
            // Fork and execute commands, including piped commands
            cmd_exec(arguments, background);
        }
    }
    return -1;  // This line should never be reached
}

// Handle changing directories
void changeDir(char *arguments[]) {
    if (arguments[1] == NULL) {
        fprintf(stderr, "cd: expected argument to \"cd\"\n");
    } else if (chdir(arguments[1]) != 0) {
        perror("cd failed");
    }
}

// Print the current working directory
void getWorkingDir() {
    char cwd[MAX_COMMAND_LINE_LEN];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    } else {
        perror("Error getting current working directory");
    }
}

// Print environment variables or a specific one
void getEnv(char *arguments[]) {
    if (arguments[1] != NULL) {
        char *value = getenv(arguments[1]);
        if (value) {
            printf("%s\n", value);
        } else {
            fprintf(stderr, "Environment variable %s not found\n", arguments[1]);
        }
    } else {
        for (int i = 0; environ[i] != NULL; i++) {
            printf("%s\n", environ[i]);
        }
    }
}

// Exit the shell
void exitShell() {
    exit(0);
}

// Set an environment variable
void setEnv(char *arguments[]) {
    if (arguments[1] == NULL || arguments[2] == NULL) {
        fprintf(stderr, "Usage: setenv VAR VALUE\n");
    } else {
        if (setenv(arguments[1], arguments[2], 1) != 0) {
            perror("setenv failed");
        }
    }
}

// Echo arguments with support for environment variables
void echoFunc(char *arguments[]) {
    for (int i = 1; arguments[i] != NULL; i++) {
        if (arguments[i][0] == '$') {
            char *env_val = getenv(arguments[i] + 1);
            if (env_val) {
                printf("%s ", env_val);
            } else {
                printf(" ");
            }
        } else {
            printf("%s ", arguments[i]);
        }
    }
    printf("\n");
}

// Parse quoted strings
void parseQuotes(char *token) {
    if (token[0] == '"' || token[0] == '\'') {
        char quote = token[0];
        int length = strlen(token);
        if (token[length - 1] == quote) {
            memmove(token, token + 1, length - 2);
            token[length - 2] = '\0';
        }
    }
}

// Function to execute commands with optional piping
void cmd_exec(char *arguments[], bool background) {
    // Separate commands by the pipe symbol and set up each as an array of arguments
    char *commands[MAX_COMMAND_LINE_ARGS][MAX_COMMAND_LINE_ARGS];
    int cmd_index = 0, arg_index = 0;

    // Split input into individual commands separated by "|"
    for (int i = 0; arguments[i] != NULL; i++) {
        if (strcmp(arguments[i], "|") == 0) {
            commands[cmd_index][arg_index] = NULL;
            cmd_index++;
            arg_index = 0;
        } else {
            commands[cmd_index][arg_index] = arguments[i];
            arg_index++;
        }
    }
    commands[cmd_index][arg_index] = NULL;  // End the last command
    int num_commands = cmd_index + 1;

    // If only one command, execute normally, otherwise handle piped execution
    if (num_commands == 1) {
        int pid = fork();
        if (pid == 0) {
            // Child process
            signal(SIGINT, SIG_DFL);  // Reset signal handling in child
            int input_fd = -1, output_fd = -1;

            if (handleIO(arguments, &input_fd, &output_fd)) {
                if (execvp(arguments[0], arguments) == -1) {
                    perror("Execution failed");
                }
            }
            exit(0);
        } else if (pid < 0) {
            perror("Fork failed");
        } else {
            if (!background) {
                cmd_pid = pid;
                int timeout_pid = fork();
                if (timeout_pid == 0) {
                    timeout_process(10, pid);  // Timeout after 10 seconds
                    exit(0);
                } else {
                    waitpid(pid, NULL, 0);
                    kill(timeout_pid, SIGINT);
                    waitpid(timeout_pid, NULL, 0);
                }
            }
        }
    } else {
        executePipedCommands(commands, num_commands);
    }
}

// Helper function to handle command pipelines
void executePipedCommands(char *commands[][MAX_COMMAND_LINE_ARGS], int num_commands) {
    int pipes[num_commands - 1][2];

    // Create pipes
    for (int i = 0; i < num_commands - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("Pipe creation failed");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < num_commands; i++) {
        int pid = fork();
        if (pid == 0) {
            // Setup I/O redirection for piping
            if (i > 0) {  // Not the first command
                dup2(pipes[i - 1][0], STDIN_FILENO);
                close(pipes[i - 1][1]);
            }
            if (i < num_commands - 1) {  // Not the last command
                dup2(pipes[i][1], STDOUT_FILENO);
                close(pipes[i][0]);
            }

            // Close all pipes in child
            for (int j = 0; j < num_commands - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            if (execvp(commands[i][0], commands[i]) == -1) {
                perror("Execution failed");
                exit(EXIT_FAILURE);
            }
        }
    }

    // Close all pipes in the parent process
    for (int i = 0; i < num_commands - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    // Wait for all processes in the pipeline
    for (int i = 0; i < num_commands; i++) {
        wait(NULL);
    }
}

// Redirect input/output if specified with < or >
bool handleIO(char *arguments[], int *input_fd, int *output_fd) {
    for (int i = 0; arguments[i] != NULL; i++) {
        if (strcmp(arguments[i], "<") == 0) {
            if (arguments[i + 1] == NULL) {
                fprintf(stderr, "Error: expected file name after '<'\n");
                return false;
            }
            *input_fd = open(arguments[i + 1], O_RDONLY);
            if (*input_fd < 0) {
                perror("Input redirection failed");
                return false;
            }
            dup2(*input_fd, STDIN_FILENO);
            close(*input_fd);
            arguments[i] = NULL;
        } else if (strcmp(arguments[i], ">") == 0) {
            if (arguments[i + 1] == NULL) {
                fprintf(stderr, "Error: expected file name after '>'\n");
                return false;
            }
            *output_fd = open(arguments[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (*output_fd < 0) {
                perror("Output redirection failed");
                return false;
            }
            dup2(*output_fd, STDOUT_FILENO);
            close(*output_fd);
            arguments[i] = NULL;
        }
    }
    return true;
}

// Timeout handler for processes that exceed the time limit
void timeout_process(int time, int pid) {
    sleep(time);
    printf("Process exceeded time limit, terminating...\n");
    kill(pid, SIGINT);
}

// Signal handler for SIGINT to prevent shell from exiting
void signal_handler(int signum) {
    if (cmd_pid != -1) {
        kill(cmd_pid, SIGINT);
    }
}
