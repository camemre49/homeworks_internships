#include "implementations.h"

void execute_pipeline(parsed_input *parsedInput) {
    int num_commands = parsedInput->num_inputs;
    int pipes[num_commands - 1][2];
    pid_t childIds[parsedInput->num_inputs];

    for (int j = 0; j < num_commands - 1; j++) {
        if (pipe(pipes[j]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < num_commands; i++) {
        pid_t pid = fork();
        childIds[i] = pid;

        if (pid == 0) { // Child process
            if (i > 0) {
                dup2(pipes[i - 1][0], STDIN_FILENO); // Redirect input from previous pipe
            }
            if (i < num_commands - 1) {
                dup2(pipes[i][1], STDOUT_FILENO); // Redirect output to current pipe
            }

            for (int p = 0; p < num_commands - 1; p++) {
                close(pipes[p][0]); // Close child read pipes
                close(pipes[p][1]); // Close child write pipes
            }

            if (parsedInput->inputs[i].type == INPUT_TYPE_SUBSHELL) { // SUBSHELL IMPLEMENTATION
                parsed_input pI;

                // If subshell includes parallel commands
                if (i != 0 && hasComma(parsedInput->inputs[i].data.subshell)) {
                    execute_parallel_with_repeater(parsedInput->inputs[i].data.subshell, &pI);
                    free_parsed_input(&pI); // Free the allocated memory
                    exit(EXIT_SUCCESS);
                } else {
                    execute_input(parsedInput->inputs[i].data.subshell, &pI);
                    free_parsed_input(&pI); // Free the allocated memory
                    exit(EXIT_SUCCESS);
                }
            }

            // Execute command
            single_input *inp = &parsedInput->inputs[i];
            execvp(inp->data.cmd.args[0], inp->data.cmd.args);

            // execvp failed
            exit(EXIT_FAILURE);

        } else if (pid < 0) { // Fork failed
            exit(EXIT_FAILURE);
        }
    }

    for (int j = 0; j < num_commands - 1; j++) {
        close(pipes[j][0]); // Close read end of pipes in parent process
        close(pipes[j][1]); // Close write end of pipes in parent process
    }

    for (int j = 0; j < num_commands; j++) {
        if (!wait(NULL)) {
            exit(EXIT_FAILURE);
        }
    }
}

void execute_sequential(parsed_input *parsedInput) {
    int num_commands = parsedInput->num_inputs;
    pid_t childIds[parsedInput->num_inputs];


    for (int i = 0; i < num_commands; i++) {
        pid_t pid = fork();
        childIds[i] = pid;

        if (pid == 0) { // Child process
            // Execute command
            single_input *inp = &parsedInput->inputs[i];
            execvp(inp->data.cmd.args[0], inp->data.cmd.args);

            // execvp failed
            exit(EXIT_FAILURE);

        } else if (pid < 0) { // Fork failed
            exit(EXIT_FAILURE);
        } else {
            if (!wait(NULL)) {
                exit(EXIT_FAILURE);
            }
        }
    }
}

void execute_parallel(parsed_input *parsedInput) {
    int num_commands = parsedInput->num_inputs;
    pid_t childIds[parsedInput->num_inputs];


    for (int i = 0; i < num_commands; i++) {
        pid_t pid = fork();
        childIds[i] = pid;

        if (pid == 0) { // Child process
            // Execute command
            single_input *inp = &parsedInput->inputs[i];
            execvp(inp->data.cmd.args[0], inp->data.cmd.args);

            // execvp failed
            exit(EXIT_FAILURE);

        } else if (pid < 0) { // Fork failed
            exit(EXIT_FAILURE);
        }
    }

    for (int j = 0; j < num_commands; j++) {
        if (!wait(NULL)) {
            exit(EXIT_FAILURE);
        }
    }
}

void execute_pipeline_from_single_input(single_input *parsedInput) {

    if (parsedInput->type == INPUT_TYPE_COMMAND) {
        // Execute command,
        char **inp = (char **) &parsedInput->data.cmd.args;
        execvp(inp[0], inp);

        // execvp failed
        exit(EXIT_FAILURE);
    }

    int num_commands = parsedInput->data.pline.num_commands;
    int pipes[num_commands - 1][2];
    pid_t childIds[num_commands];

    for (int j = 0; j < num_commands - 1; j++) {
        if (pipe(pipes[j]) == -1) {
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < num_commands; i++) {
        pid_t pid = fork();
        childIds[i] = pid;

        if (pid == 0) { // Child process
            if (i > 0) {
                dup2(pipes[i - 1][0], STDIN_FILENO); // Redirect input from previous pipe
            }
            if (i < num_commands - 1) {
                dup2(pipes[i][1], STDOUT_FILENO); // Redirect output to current pipe
            }

            for (int p = 0; p < num_commands - 1; p++) {
                close(pipes[p][0]); // Close child read pipes
                close(pipes[p][1]); // Close child write pipes
            }

            // Execute command,
            command *inp = &parsedInput->data.pline.commands[i];
            execvp(inp->args[0], inp->args);

            // execvp failed
            exit(EXIT_FAILURE);

        } else if (pid < 0) { // Fork failed
            exit(EXIT_FAILURE);
        }
    }

    for (int j = 0; j < num_commands - 1; j++) {
        close(pipes[j][0]); // Close read end of pipes in parent process
        close(pipes[j][1]); // Close write end of pipes in parent process
    }

    for (int j = 0; j < num_commands; j++) {
        if (!wait(NULL)) {
            exit(EXIT_FAILURE);
        }
    }
}

int is_pipelined(parsed_input *parsedInput) {
    for (int i = 0; i < parsedInput->num_inputs; i++) {
        if (parsedInput->inputs[i].type == INPUT_TYPE_PIPELINE) {
            return 1;
        }
    }
    return 0;
}

int is_subshell(parsed_input *parsedInput) {
    for (int i = 0; i < parsedInput->num_inputs; i++) {
        if (parsedInput->inputs[i].type == INPUT_TYPE_SUBSHELL) {
            return 1;
        }
    }
    return 0;
}

int hasComma(char *str) {
    while (*str != '\0') {
        if (*str == ',') {
            return 1; // comma found
        }
        str++; // move to the next character
    }
    return 0; // comma not found
}

