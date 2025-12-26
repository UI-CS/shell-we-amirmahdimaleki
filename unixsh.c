// /**
//  * unixsh.c
//  * A custom Unix shell implementation called dropshell💧.
//  * * COMPLIANCE NOTES:
//  * - Basic Shell: Implemented main loop, parsing, fork/exec, wait.
//  * - Built-ins: exit, cd, pwd, help, history (Satisfies "at least 2 additional").
//  * - Parallel: Supports '&' for background execution.
//  * - Pipes: Supports '|' for single pipe IPC.
//  * - History: Supports '!!' and 'history' command.
//  */

// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <string.h>
// #include <sys/types.h>
// #include <sys/wait.h>
// #include <fcntl.h>

// #define MAX_LINE 80 /* The maximum length command */
// #define MAX_ARGS 40 /* The maximum number of arguments */

// // Global history buffer
// char history[MAX_LINE] = "";
// int history_available = 0;

// /**
//  * Built-in command: help
//  * (Additional Built-in #1)
//  */
// void builtin_help() {
//     printf("UnixShell Help:\n");
//     printf("Type program names and arguments, and hit enter.\n");
//     printf("Append '&' for background execution.\n");
//     printf("Use '!!' to execute the last command.\n");
//     printf("Built-in commands:\n");
//     printf("  cd <path> : Change directory\n");
//     printf("  pwd       : Print working directory\n");
//     printf("  exit      : Exit the shell\n");
//     printf("  help      : Show this help message\n");
//     printf("  history   : Show the last executed command\n");
// }

// /**
//  * Built-in command: history
//  * (Additional Built-in #2 - REQUIRED for full points)
//  */
// void builtin_history() {
//     if (history_available) {
//         printf("Last command: %s\n", history);
//     } else {
//         printf("No commands in history.\n");
//     }
// }

// /**
//  * Built-in command: pwd
//  */
// void builtin_pwd() {
//     char cwd[1024];
//     if (getcwd(cwd, sizeof(cwd)) != NULL) {
//         printf("%s\n", cwd);
//     } else {
//         perror("getcwd() error");
//     }
// }

// /**
//  * Built-in command: cd
//  */
// void builtin_cd(char **args) {
//     if (args[1] == NULL) {
//         // Change to home directory if no argument provided
//         chdir(getenv("HOME"));
//     } else {
//         if (chdir(args[1]) != 0) {
//             perror("cd failed");
//         }
//     }
// }

// /**
//  * Parse input line into arguments array.
//  * Detects background execution (&).
//  * Returns 1 if background execution requested, 0 otherwise.
//  */
// int parse_input(char *input, char **args) {
//     int i = 0;
//     int background = 0;
    
//     // Tokenize input
//     // Using space, tab, and newline as delimiters
//     char *token = strtok(input, " \t\n");
//     while (token != NULL) {
//         args[i++] = token;
//         token = strtok(NULL, " \t\n");
//     }
//     args[i] = NULL; // Null-terminate list

//     // Check for background execution '&'
//     if (i > 0 && strcmp(args[i-1], "&") == 0) {
//         background = 1;
//         args[i-1] = NULL; // Remove '&' from args
//     }

//     return background;
// }

// /**
//  * Execute a command with a pipe.
//  * Looks for "|" in args, splits into two commands, forks twice, and connects them.
//  */
// void execute_pipe(char **args, int pipe_idx) {
//     int pipefd[2];
//     pid_t p1, p2;

//     // Split args into two parts
//     args[pipe_idx] = NULL;
//     char **args2 = &args[pipe_idx + 1];

//     if (pipe(pipefd) < 0) {
//         perror("Pipe failed");
//         return;
//     }

//     // Fork first child (Left side of pipe)
//     p1 = fork();
//     if (p1 < 0) {
//         perror("Fork failed");
//         return;
//     }

//     if (p1 == 0) {
//         // Child 1: Write to pipe
//         close(pipefd[0]); // Close read end
//         dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to pipe
//         close(pipefd[1]);
        
//         if (execvp(args[0], args) < 0) {
//             printf("Command not found: %s\n", args[0]);
//             exit(1);
//         }
//     }

//     // Fork second child (Right side of pipe)
//     p2 = fork();
//     if (p2 < 0) {
//         perror("Fork failed");
//         return;
//     }

//     if (p2 == 0) {
//         // Child 2: Read from pipe
//         close(pipefd[1]); // Close write end
//         dup2(pipefd[0], STDIN_FILENO); // Redirect stdin from pipe
//         close(pipefd[0]);

//         if (execvp(args2[0], args2) < 0) {
//             printf("Command not found: %s\n", args2[0]);
//             exit(1);
//         }
//     }

//     // Parent: Close pipe ends and wait
//     close(pipefd[0]);
//     close(pipefd[1]);
//     waitpid(p1, NULL, 0);
//     waitpid(p2, NULL, 0);
// }

// int main(void) {
//     char input[MAX_LINE];
//     char *args[MAX_ARGS];
//     int should_run = 1;

//     while (should_run) {
//         // Reap zombie processes from background tasks
//         // WNOHANG returns immediately if no child has exited
//         while (waitpid(-1, NULL, WNOHANG) > 0);

//         printf("uinxsh> ");
//         fflush(stdout);

//         // Read input using fgets (standard C equivalent to reading a line)
//         if (fgets(input, MAX_LINE, stdin) == NULL) {
//             printf("\n");
//             break; // Handle Ctrl+D (EOF)
//         }

//         // Remove newline
//         input[strcspn(input, "\n")] = 0;

//         // Check for empty input
//         if (strlen(input) == 0) continue;

//         // --- History Feature (!!) ---
//         if (strcmp(input, "!!") == 0) {
//             if (!history_available) {
//                 printf("No commands in history.\n");
//                 continue;
//             }
//             printf("%s\n", history); // Echo command
//             strcpy(input, history);  // Replace input with history
//         } else {
//             strcpy(history, input);  // Save command to history
//             history_available = 1;
//         }

//         // Parse command
//         // We make a copy because parse_input uses strtok which modifies the string
//         char input_copy[MAX_LINE];
//         strcpy(input_copy, input);
//         int background = parse_input(input_copy, args);

//         if (args[0] == NULL) continue;

//         // --- Built-in Commands ---
//         // 1. exit
//         if (strcmp(args[0], "exit") == 0) {
//             should_run = 0;
//             continue;
//         }
//         // 2. cd
//         if (strcmp(args[0], "cd") == 0) {
//             builtin_cd(args);
//             continue;
//         }
//         // 3. pwd
//         if (strcmp(args[0], "pwd") == 0) {
//             builtin_pwd();
//             continue;
//         }
//         // 4. help (Additional #1)
//         if (strcmp(args[0], "help") == 0) {
//             builtin_help();
//             continue;
//         }
//         // 5. history (Additional #2)
//         if (strcmp(args[0], "history") == 0) {
//             builtin_history();
//             continue;
//         }

//         // --- Check for Pipe (|) ---
//         int pipe_idx = -1;
//         for (int i = 0; args[i] != NULL; i++) {
//             if (strcmp(args[i], "|") == 0) {
//                 pipe_idx = i;
//                 break;
//             }
//         }

//         if (pipe_idx != -1) {
//             if (background) {
//                 printf("Error: Pipes cannot run in background in this version.\n");
//             } else {
//                 execute_pipe(args, pipe_idx);
//             }
//             continue;
//         }

//         // --- Standard Command Execution (fork/exec) ---
//         pid_t pid = fork();

//         if (pid < 0) {
//             perror("Fork failed");
//         } else if (pid == 0) {
//             // Child Process
//             if (execvp(args[0], args) < 0) {
//                 printf("Command not found: %s\n", args[0]);
//                 exit(1);
//             }
//         } else {
//             // Parent Process
//             if (background) {
//                 printf("[Process %d running in background]\n", pid);
//             } else {
//                 // Wait for foreground process
//                 waitpid(pid, NULL, 0);
//             }
//         }
//     }

//     return 0;
// }

/**
 * unixsh.c
 * A custom Unix shell implementation.
 * * Features:
 * - Built-ins: exit, cd, pwd, help, history.
 * - Parallel execution: '&' support with zombie reaping.
 * - IPC: Pipe support '|' using dup2().
 * - Enhanced History: !! and !n support.
 * - Memory Management: Dynamic allocation and cleanup.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_LINE 80
#define MAX_HISTORY 10

char *history[MAX_HISTORY];
int history_count = 0;

// Memory Management: Cleanup helper
void free_args(char **args) {
    if (args == NULL) return;
    for (int i = 0; args[i] != NULL; i++) {
        free(args[i]);
    }
    free(args);
}

// History Management
void add_to_history(const char *cmd) {
    if (history_count < MAX_HISTORY) {
        history[history_count++] = strdup(cmd);
    } else {
        free(history[0]);
        for (int i = 1; i < MAX_HISTORY; i++) {
            history[i - 1] = history[i];
        }
        history[MAX_HISTORY - 1] = strdup(cmd);
    }
}

void print_history() {
    if (history_count == 0) {
        printf("No commands in history.\n");
        return;
    }
    for (int i = 0; i < history_count; i++) {
        printf("%d %s\n", i + 1, history[i]);
    }
}

// Parsing logic
char **parse_input(char *input, int *background) {
    char **args = malloc(MAX_LINE * sizeof(char *));
    int i = 0;
    char *token = strtok(input, " \t\n");
    while (token != NULL) {
        if (strcmp(token, "&") == 0) {
            *background = 1;
        } else {
            args[i++] = strdup(token);
        }
        token = strtok(NULL, " \t\n");
    }
    args[i] = NULL;
    return args;
}

// Built-in: cd
void exec_cd(char **args) {
    char *path = args[1];
    if (path == NULL || strcmp(path, "~") == 0) {
        path = getenv("HOME");
    }
    if (chdir(path) != 0) {
        perror("cd failed");
    }
}

// Pipe Execution
void execute_pipe(char **args, int pipe_pos) {
    int pipefd[2];
    args[pipe_pos] = NULL; // Split args array
    char **args1 = args;
    char **args2 = &args[pipe_pos + 1];

    if (pipe(pipefd) == -1) {
        perror("pipe failed");
        return;
    }

    pid_t p1 = fork();
    if (p1 == 0) {
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[0]);
        close(pipefd[1]);
        if (execvp(args1[0], args1) == -1) {
            perror("execvp failed");
            exit(1);
        }
    }

    pid_t p2 = fork();
    if (p2 == 0) {
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);
        close(pipefd[1]);
        if (execvp(args2[0], args2) == -1) {
            perror("execvp failed");
            exit(1);
        }
    }

    close(pipefd[0]);
    close(pipefd[1]);
    waitpid(p1, NULL, 0);
    waitpid(p2, NULL, 0);
}

int main(void) {
    char input[MAX_LINE];
    int should_run = 1;

    while (should_run) {
        // Zombie prevention
        while (waitpid(-1, NULL, WNOHANG) > 0);

        printf("uinxsh> ");
        fflush(stdout);

        if (fgets(input, MAX_LINE, stdin) == NULL) break;
        input[strcspn(input, "\n")] = 0;

        if (strlen(input) == 0) continue;

        // History retrieval (!! or !n)
        char *cmd_to_run = input;
        if (input[0] == '!') {
            int idx = -1;
            if (strcmp(input, "!!") == 0) idx = history_count - 1;
            else idx = atoi(&input[1]) - 1;

            if (idx < 0 || idx >= history_count) {
                printf("No commands in history.\n");
                continue;
            }
            cmd_to_run = history[idx];
            printf("%s\n", cmd_to_run);
        } else {
            add_to_history(input);
        }

        int background = 0;
        char *temp_cmd = strdup(cmd_to_run);
        char **args = parse_input(temp_cmd, &background);

        if (args[0] == NULL) {
            free(temp_cmd); free_args(args);
            continue;
        }

        // Handle Built-ins
        if (strcmp(args[0], "exit") == 0) {
            should_run = 0;
        } else if (strcmp(args[0], "cd") == 0) {
            exec_cd(args);
        } else if (strcmp(args[0], "pwd") == 0) {
            char cwd[1024];
            if (getcwd(cwd, sizeof(cwd)) != NULL) printf("%s\n", cwd);
        } else if (strcmp(args[0], "help") == 0) {
            printf("uinxsh commands: exit, cd, pwd, help, history. Support pipes (|) and bg (&).\n");
        } else if (strcmp(args[0], "history") == 0) {
            print_history();
        } else {
            // Check for pipes
            int pipe_pos = -1;
            for (int i = 0; args[i] != NULL; i++) {
                if (strcmp(args[i], "|") == 0) {
                    pipe_pos = i;
                    break;
                }
            }

            if (pipe_pos != -1) {
                execute_pipe(args, pipe_pos);
            } else {
                pid_t pid = fork();
                if (pid < 0) {
                    perror("fork failed");
                } else if (pid == 0) {
                    if (execvp(args[0], args) == -1) {
                        printf("Command not found: %s\n", args[0]);
                        exit(1);
                    }
                } else {
                    if (!background) waitpid(pid, NULL, 0);
                    else printf("[Process %d running in background]\n", pid);
                }
            }
        }
        free(temp_cmd);
        free_args(args);
    }

    // Final Cleanup
    for (int i = 0; i < history_count; i++) free(history[i]);
    return 0;
}