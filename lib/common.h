/**
 * @file common.h
 * @brief Utility functions for string validation and safe copying.
 *
 * This module contains helper functions used throughout the client code
 * for validating input formats and performing safe string operations.
 * These functions ensure that inputs meet protocol constraints and avoid
 * common issues like buffer overflows or invalid characters.
 * 
 * Functions include validation for alphanumeric identifiers, printable
 * character checks, and message content validation, as well as a
 * safe string copy function.
 * 
 * @author Samuel Stefanik (xstefas00)
 * @date 2025-04-19
 */
#ifndef COMMON_H
#define COMMON_H

#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdint.h>
#include <errno.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ctype.h>
#include <signal.h>
#include <sys/time.h>
#include <termios.h>

#define MAX_NAME_SIZE 20
#define MAX_SECRET_SIZE 128
#define MAX_MSG_SIZE 60000
#define ZERO_BYTE 1
#define BUFFER_SIZE UINT16_MAX

/**
 * @brief Enum representing supported message types for communication.
 */
typedef enum msg_type{
    CONFIRM = 0x00,
    REPLY,
    AUTH,
    JOIN,
    MSG,
    NREPLY,
    ERROR,
    LOCAL,
    PING = 0xFD,
    ERR,
    BYE
} msg_type_t;

/**
 * @brief Structure holding all relevant information about the client.
 */
typedef struct client {
    uint16_t msg_id;                                     /**< Message ID used to identify sent messages. */
    char username[MAX_NAME_SIZE + ZERO_BYTE];            /**< Username of the client. */
    char channel_ID[MAX_NAME_SIZE + ZERO_BYTE];          /**< ID of the channel the client is connected to. */
    char secret[MAX_SECRET_SIZE + ZERO_BYTE];            /**< Secret or token used for authentication. */
    char display_name[MAX_NAME_SIZE + ZERO_BYTE];        /**< Display name send in messages. */
    char msg_content[MAX_MSG_SIZE + ZERO_BYTE];          /**< Buffer for message content. */
    struct addrinfo *res;                                /**< Address info for communication. */
} client_t;

// Global client structure used throughout the program
extern client_t client;

/**
 * @brief Macro to check if a pointer is NULL and handle the error.
 *
 * @param ptr Pointer to check.
 */
#define PTR_CHECK(ptr)                                          \
    if (!ptr) {                                                 \
        fprintf(stderr, "ERROR: using NULL pointer\n");         \
        return EXIT_FAILURE;                                    \
    }

/**
 * @brief Validates that a string contains only alphanumeric characters,
 *        hyphens, or underscores, and is within the given length.
 * 
 * @param str The string to validate.
 * @param max_len The maximum allowed length for the string.
 * @return true if valid, false otherwise.
 * 
 * @note This functions was created using ChatGPT
 */
bool is_valid_alphanum_underscore(char *str, int max_len);

/**
 * @brief Validates that a string contains only printable ASCII characters
 *        (excluding space), and is within the given length.
 * 
 * @param str The string to validate.
 * @param max_len The maximum allowed length for the string.
 * @return true if valid, false otherwise.
 * 
 * @note This functions was created using ChatGPT
 */
bool is_valid_printable(char *str, int max_len);

/**
 * @brief Validates that a string contains valid message content,
 *        including newlines and printable ASCII characters (including space),
 *        and is within the given length.
 * 
 * @param str The string to validate.
 * @param max_len The maximum allowed length for the string.
 * @return true if valid, false otherwise.
 * 
 * @note This functions was created using ChatGPT
 */
bool is_valid_msg_content(char *str, int max_len);

/**
 * @brief Safely copies a string with null-termination.
 * 
 * Copies at most (size - 1) characters and ensures the destination
 * string is null-terminated.
 * 
 * @param dest Destination buffer.
 * @param src Source string.
 * @param size Size of the destination buffer.
 * 
 * @note This functions was created using ChatGPT
 */
void copy(char *dest, char *src, int size);

#endif
