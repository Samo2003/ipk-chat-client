/**
 * @file input.h
 * @brief Handles parsing and validation of user input from stdin.
 *
 * This module provides functionality to interpret user commands and messages.
 * It validates syntax and content according to expected formats and sets
 * appropriate values in the `client` structure for further processing.
 * 
 * @author Samuel Stefanik (xstefas00)
 * @date 2025-04-19
 */
#ifndef INPUT_H
#define INPUT_H

#include "common.h"

#define COMMAND_COUNT 4     /**< Total number of supported commands. */

/**
 * @brief Structure representing a command and its parsing function.
 */
typedef struct {
    char *cmd;                          /**< Command string. */
    msg_type_t (*parse)(char **args);   /**< Function to handle the command. */
} command_t;

// Global interrupt flag
extern volatile int interrupt;

/**
 * @brief Parses a full line of user input from stdin.
 * 
 * This function supports command parsing (starting with /) or sending
 * general messages. It performs input validation and handles truncation
 * if the message exceeds MAX_MSG_SIZE.
 * 
 * @return A value of type msg_type_t representing the parsed message type.
 */
msg_type_t parse_user_input(void);

#endif
