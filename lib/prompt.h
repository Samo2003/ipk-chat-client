/**
 * @file prompt.h
 * @brief Handles waiting for and processing user or server input.
 *
 * This module contains logic for managing input sources (user and server) using `select()`,
 * and handling timeouts when a server response is expected.
 * 
 * The function `await()` blocks until input is available from either the server socket or
 * the user via standard input, depending on the state of communication. If a response
 * is required from the server, a timeout is enforced.
 * 
 * @author Samuel Stefanik (xstefas00)
 * @date 2025-04-19
 */
#ifndef PROMPT_H
#define PROMPT_H

#include "comm.h"
#include "input.h"

/**
 * @enum input_type_t
 * @brief Identifies the source of the input.
 */
typedef enum {
    USER,   /**< Input came from the user. */
    SERVER  /**< Input came from the server. */
} input_type_t;

/**
 * @struct prompt
 * @brief Represents one unit of input, including its source and type.
 */
typedef struct prompt {
    input_type_t input_type; /**< Indicates whether the input came from the user or server. */
    msg_type_t type;         /**< The type of message received or entered. */
} prompt_t;

/**
 * @brief Waits for and retrieves the next user or server input.
 *
 * This function blocks using `select()` until input is ready from either:
 *  - the server socket,
 *  - or standard input (if no `REPLY` is awaited).
 * 
 * If a message is already stored, it is returned immediately.
 * If a timeout occurs when waiting for a required server response, an error prompt is returned.
 *
 * @return A `prompt_t` struct containing the input type (USER or SERVER) and message type.
 */
prompt_t await(void);

#endif
