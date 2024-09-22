#include "parser.h"
#include "implementations.h"


// Empty SIGPIPE handler function
void sigpipe_handler(int signum) {
    // Do nothing. This is just to suspend pipe signal on large inputs.
}

int main() {
    char line[BUFSIZ];
    signal(SIGPIPE, sigpipe_handler);
    parsed_input parsedInput; // Declare parsed_input struct, no need for dynamic allocation
    while (1) {
        if (fgets(line, sizeof(line), stdin) == NULL) {
            free_parsed_input(&parsedInput);
            return 0; // Continue to the next iteration if input reading fails
        }

        if (strcmp(line, " quit\n") == 0 || strcmp(line, "quit\n") == 0) {
            free_parsed_input(&parsedInput);
            return 0;
        }

        execute_input(line, &parsedInput);
    }
    return 0;
}

void execute_input(char *input, parsed_input *parsedInputP) {
    parsed_input parsedInput = *parsedInputP;
    parse_line(input, &parsedInput);
    SEPARATOR firstSeparator = parsedInput.num_inputs < 1 ? SEPARATOR_NONE : parsedInput.separator;
    int has_pipe = is_pipelined(&parsedInput);

    switch (firstSeparator) {
        case SEPARATOR_NONE:
        case SEPARATOR_PIPE:
            execute_pipeline(&parsedInput);
            break;
        case SEPARATOR_SEQ:
            if (has_pipe) {
                pid_t p;
                for (int inpId = 0; inpId < parsedInput.num_inputs; ++inpId) {
                    p = fork();
                    if (p == 0) {
                        execute_pipeline_from_single_input(&(parsedInput.inputs[inpId]));
                        exit(EXIT_SUCCESS);
                    } else {
                        waitpid(p, 0, 0);
                    }
                }
            } else {
                execute_sequential(&parsedInput);
            }
            break;
        case SEPARATOR_PARA:
            if (has_pipe) {
                for (int inpId = 0; inpId < parsedInput.num_inputs; ++inpId) {
                    if (fork() == 0) {
                        execute_pipeline_from_single_input(&(parsedInput.inputs[inpId]));
                        exit(EXIT_SUCCESS);
                    }
                }
                for (int inpId = 0; inpId < parsedInput.num_inputs; ++inpId) {
                    wait(NULL);
                }
            } else {
                execute_parallel(&parsedInput);
            }
            break;
        default:
            break;
    }
}


void execute_parallel_with_repeater(char *input, parsed_input* parsedInput) {
    parse_line(input, parsedInput);
    int num_commands = parsedInput->num_inputs;
    pid_t childIds[num_commands];
    int pipes[num_commands][2];

    for(int i = 0; i < num_commands; i++) {
        if (pipe(pipes[i]) == -1) {
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < num_commands; i++) {
        pid_t pid = fork();
        childIds[i] = pid;

        if (pid == 0) { // Child process
            // Close unnecessary pipe ends
            for (int j = 0; j < num_commands; j++) {
                if(j != i) {
                    close(pipes[j][0]); // Close read end of other pipes
                    close(pipes[j][1]); // Close write end of other pipes
                }
            }

            dup2(pipes[i][0], STDIN_FILENO); // Redirect stdin to read end of pipe
            close(pipes[i][1]); // Close write end of the current pipe
            close(pipes[i][0]); // Close read end of the current pipe

            // Execute command
            single_input *inp = &parsedInput->inputs[i];
            execvp(inp->data.cmd.args[0], inp->data.cmd.args);

            // execvp failed
            exit(EXIT_FAILURE);
        } else if (pid < 0) { // Fork failed
            exit(EXIT_FAILURE);
        }
    }
    // Close reads
    for (int i = 0; i < num_commands; i++) {
        close(pipes[i][0]);
    }

    // Repeater process
    if (fork() == 0) {
        char buf[BUFSIZ];
        ssize_t nby;
        while ((nby = read(STDIN_FILENO, buf, sizeof(buf))) > 0) {
            for (int i = 0; i < num_commands; i++) {
                write(pipes[i][1], buf, nby); // Write to each pipe
            }
        }
        if (nby == -1) {
            exit(EXIT_FAILURE);
        }

        // Close write ends of all pipes when EOF is received
        for (int i = 0; i < num_commands; i++) {
            close(pipes[i][1]);
        }
        exit(EXIT_SUCCESS);
    }

    // Close all pipe ends in the parent process
    for (int i = 0; i < num_commands; i++) {
        close(pipes[i][1]); // Close write end
    }

    // Wait for all child processes to finish, parallel commands + repeater
    for (int j = 0; j < num_commands + 1; j++) {
        if (waitpid(childIds[j], NULL, 0) == -1) {
            exit(EXIT_FAILURE);
        }
    }

    // Wait for repeater process to finish
    if (wait(NULL) == -1) {
        exit(EXIT_FAILURE);
    }
}