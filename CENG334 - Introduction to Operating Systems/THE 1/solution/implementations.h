//
// Created by emre on 05.04.2024.
//

#ifndef HW_IMPLEMENTATIONS_H
#define HW_IMPLEMENTATIONS_H

#include "parser.h"
#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

/**
 * Pipelined execution
 * Example input:
 *              ps aux | grep python | wc -l
 * @param parsedInput
 */
void execute_pipeline(parsed_input * parsedInput);

/**
 * Sequential execution
 * Example input:
 *              echo "Hello"; sleep 5; echo "World"
 * @param parsedInput
 */
void execute_sequential(parsed_input * parsedInput);

/**
 * Parallel execution
 * Example input:
 *             echo "Hello", sleep 5, echo "World"
 * @param parsedInput
 */
void execute_parallel(parsed_input * parsedInput);

/**
 * Pipeline execution by taking single_input
 * @param parsedInput
 */
void execute_pipeline_from_single_input(single_input * parsedInput);

/**
 * Return 1 if any of the commands is pipelined
 */
int is_pipelined(parsed_input * parsedInput);

/**
 * Return 1 if includes subshell
 */
int is_subshell(parsed_input * parsedInput);

/**
 * Checks if there is a comma in the char*
 */
int hasComma(char *str);


/**
 * Execute a general input line
 * @param input
 */
void execute_input(char * input, parsed_input* parsedInputP);


/**
 * parallel subshell with repeater
 * @param input
 * @param parsedInputP
 */
void execute_parallel_with_repeater(char * input, parsed_input* parsedInputP);


#endif //HW_IMPLEMENTATIONS_H
