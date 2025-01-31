#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

void execute_command(char *command) {
    char *args[100];
    int arg_count = 0;
    char *token = strtok(command, " ");

    while (token != NULL) {
        args[arg_count++] = token;
        token = strtok(NULL, " ");
    }
    args[arg_count] = NULL;

    if (arg_count == 0) return;

    if (strcmp(args[0], "cd") == 0) {
        if (arg_count < 2) {
            fprintf(stderr, "cd: missing argument\n");
        } else if (chdir(args[1]) != 0) {
            perror("cd");
        }
        return;
    }

    if (strcmp(args[0], "grep") == 0) {
        for (int i = 1; i < arg_count; i++) {
            if (args[i][0] == '"') {
                memmove(args[i], args[i] + 1, strlen(args[i]));
                args[i][strlen(args[i]) - 1] = '\0';
            }
        }
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
    } else if (pid == 0) {
        execvp(args[0], args);
        perror("execvp");
        exit(EXIT_FAILURE);
    } else {
        wait(NULL);
    }
}

void handle_piping(char *command) {
    char *commands[10];
    int command_count = 0;
    char *token = strtok(command, "|");

    while (token != NULL && command_count < 10) {
        commands[command_count++] = token;
        token = strtok(NULL, "|");
    }

    int pipes[2], in_fd = 0;

    for (int i = 0; i < command_count; ++i) {
        pipe(pipes);
        pid_t pid = fork();

        if (pid == 0) {
            dup2(in_fd, STDIN_FILENO);
            if (i < command_count - 1) dup2(pipes[1], STDOUT_FILENO);
            close(pipes[0]);
            execute_command(commands[i]);
            exit(EXIT_FAILURE);
        } else {
            wait(NULL);
            close(pipes[1]);
            in_fd = pipes[0];
        }
    }
}

void handle_redirection(char *command) {
    char *output_file = NULL;
    int append = 0;

    char *redir = strstr(command, ">>");
    if (redir) {
        append = 1;
        *redir = '\0';
        output_file = strtok(redir + 2, " ");
    } else {
        redir = strstr(command, ">");
        if (redir) {
            *redir = '\0';
            output_file = strtok(redir + 1, " ");
        }
    }

    if (output_file) {
        int fd = open(output_file, O_WRONLY | O_CREAT | (append ? O_APPEND : O_TRUNC), 0644);
        if (fd < 0) {
            perror("open");
            return;
        }

        pid_t pid = fork();
        if (pid == 0) {
            dup2(fd, STDOUT_FILENO);
            close(fd);
            execute_command(command);
            exit(EXIT_FAILURE);
        } else {
            wait(NULL);
            close(fd);
        }
    } else {
        execute_command(command);
    }
}

void handle_compound_commands(char *command) {
    char *subcommands[10];
    int command_count = 0;
    char *token = strtok(command, ";");

    while (token != NULL && command_count < 10) {
        subcommands[command_count++] = token;
        token = strtok(NULL, ";");
    }

    for (int i = 0; i < command_count; ++i) {
        execute_command(subcommands[i]);
    }
}

void handle_and_commands(char *command) {
    char *subcommands[10];
    int command_count = 0;
    char *token = strtok(command, "&&");

    while (token != NULL && command_count < 10) {
        subcommands[command_count++] = token;
        token = strtok(NULL, "&&");
    }

    int status = 0;
    for (int i = 0; i < command_count; ++i) {
        execute_command(subcommands[i]);
        if (status != 0) {
            break;
        }
        wait(&status);
    }
}

int main() {
    char username[32], hostname[32], cwd[256], command[1024];

    getlogin_r(username, sizeof(username));
    gethostname(hostname, sizeof(hostname));

    while (1) {
        getcwd(cwd, sizeof(cwd));

        printf("[%s@%s:%s]$ ", username, hostname, cwd);
        fflush(stdout);

        if (!fgets(command, sizeof(command), stdin)) break;
        command[strcspn(command, "\n")] = '\0';

        if (strcmp(command, "exit") == 0) break;

        if (strstr(command, "|")) {
            handle_piping(command);
        }
        else if (strstr(command, ">")) {
            handle_redirection(command);
        }
        else if (strstr(command, ";")) {
            handle_compound_commands(command);
        }
        else if (strstr(command, "&&")) {
            handle_and_commands(command);
        }
        else {
            execute_command(command);
        }
    }

    return 0;
}