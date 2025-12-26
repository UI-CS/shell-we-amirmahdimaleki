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