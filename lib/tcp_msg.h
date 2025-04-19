/**
 * @file tcp_msg.h
 * @brief Contains function definitions to build and parse TCP protocol variant messages.
 *
 * This module defines functions for constructing and interpreting TCP messages
 * according to a custom text-based chat protocol.
 * 
 * @author Samuel Stefanik (xstefas00)
 * @date 2025-04-19
 */
#ifndef TCP_MSG_H
#define TCP_MSG_H

#include "args.h"
#include "buffer.h"

#define TEMP_COUNT 7 /**< Number of supported TCP message templates. */

/**
 * @typedef parse
 * @brief Function pointer type for message parsers.
 * 
 * Functions of this type take a message buffer and return the corresponding message type.
 */
typedef msg_type_t (*parse)(char *buffer);

/**
 * @struct msg_temp
 * @brief Structure representing a TCP message template and parser.
 * 
 * This structure maps a message type to its corresponding format template, 
 * its expected length, and the function used to parse it.
 */
typedef struct msg_temp {
    msg_type_t type;         /**< Type of the message */
    const char *temp;        /**< Format template string for building the message. */
    size_t len;              /**< Length of the format string to compare. */
    parse parse_func;        /**< Function used to parse this type of message. */
} tcp_msg_temp_t;

// Array of message templates and their parsers for all supported TCP message types.
extern const tcp_msg_temp_t msg_temp[TEMP_COUNT];

/**
 * @brief Constructs a TCP message string based on type and current client data.
 * 
 * @param type Message type to construct (AUTH, JOIN, MSG, etc.).
 * @param msg_len Pointer to store the resulting message length.
 * @return Dynamically allocated message string or NULL on failure.
 */
char *tcp_build_msg(msg_type_t type, int *msg_len);

/**
 * @brief Parses a successful REPLY message.
 * 
 * @param buffer Message string to parse.
 * @return Message type REPLY on success, ERROR otherwise.
 */
msg_type_t parse_reply(char *buffer);

/**
 * @brief Parses an unsuccessful REPLY message (NOK).
 * 
 * @param buffer Message string to parse.
 * @return Message type NREPLY on failure, ERROR otherwise.
 */
msg_type_t parse_nreply(char *buffer);

/**
 * @brief Handles an unexpected message.
 * 
 * @param buffer Message string to print for debugging.
 * @return Always returns ERROR.
 */
msg_type_t parse_unexpected(char *buffer);

/**
 * @brief Parses an incoming MSG message.
 * 
 * @param buffer Message string to parse.
 * @return Message type MSG on success, ERROR otherwise.
 */
msg_type_t parse_msg(char *buffer);

/**
 * @brief Parses an incoming ERR message.
 * 
 * @param buffer Message string to parse.
 * @return Message type ERR on success, ERROR otherwise.
 */
msg_type_t parse_err(char *buffer);

/**
 * @brief Parses an incoming BYE message.
 * 
 * @param buffer Message string to parse.
 * @return Message type BYE on success, ERROR otherwise.
 */
msg_type_t parse_bye(char *buffer);

#endif
