/**
 * @file buffer.h
 * @brief Header for TCP receive buffer handling logic.
 *
 * Provides an interface for appending data received from the socket into a dynamic
 * buffer and extracting complete messages terminated by "\r\n".
 *
 * @author: Samuel Stefanik (xstefas00)
 * @date: 2025-04-19
 */
#ifndef BUFFER_H
#define BUFFER_H

#include "common.h"

/**
 * @brief Structure representing a dynamic buffer for accumulating received data.
 */
typedef struct msg_buffer_t {
    char *msg;      /**< Pointer to allocated memory for message data. */
    int msg_len;    /**< Number of valid bytes currently stored. */
    int size;       /**< Total allocated size of the buffer. */
} msg_buffer_t;

// Global instance of the receive buffer
extern msg_buffer_t msg_buffer;

/**
 * @brief Appends raw message data to the buffer.
 *
 * Automatically reallocates memory if the buffer is not large enough.
 *
 * @param msg The new data to append.
 * @param msg_len Length of the data.
 * @return true on success, false on memory allocation failure.
 * 
 * @note This functions was created using ChatGPT
 */
bool buffer_append(char *msg, int msg_len);

/**
 * @brief Frees the memory used by the buffer and resets its state.
 * 
 * @note This functions was created using ChatGPT
 */
void buffer_destroy(void);

/**
 * @brief Extracts a complete message from the buffer, if available.
 *
 * A message is considered complete if it ends with "\r\n".
 *
 * @return Dynamically allocated message string (caller must free), or NULL if incomplete.
 * 
 * @note This functions was created using ChatGPT
 */
char *buffer_get_msg(void);

/**
 * @brief Checks if the message buffer is currently empty.
 *
 * @return true if empty, false otherwise.
 * 
 * @note This functions was created using ChatGPT
 */
bool buffer_is_empty(void);

#endif
