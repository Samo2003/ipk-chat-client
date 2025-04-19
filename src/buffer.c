/**
 * @file buffer.c
 * @brief Implements a simple message buffer for processing TCP message fragments.
 *
 * The buffer accumulates data received from the socket until a full message
 * (terminated by CRLF "\r\n") is received. Once a message is complete, it can
 * be extracted and processed, with the buffer maintaining remaining data.
 *
 * Author: Samuel Stefanik (xstefas00)
 * Date: 2025-04-19
 */
#include "../lib/buffer.h"

// Global instance of the receive buffer
msg_buffer_t msg_buffer = {0};

/**
 * @brief Finds the end of a complete message marked by "\r\n".
 *
 * @return Pointer to the '\r' character if found, NULL otherwise.
 * 
 * @note This functions was created using ChatGPT
 */
static char *crlf(void) {
    for (int i = 0; i < msg_buffer.msg_len - 1; ++i) {
        if (msg_buffer.msg[i] == '\r' && msg_buffer.msg[i+1] == '\n') {
            return &msg_buffer.msg[i];
        }
    }
    return NULL;
}

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
bool buffer_append(char *msg, int msg_len) {
    if (msg_buffer.msg_len + msg_len > msg_buffer.size) {
        int new_size = msg_buffer.size + BUFFER_SIZE;
        char *new_msg = realloc(msg_buffer.msg, new_size);
        if (!new_msg) {
            return false;
        }
        msg_buffer.msg = new_msg;
        msg_buffer.size = new_size;
    }
    copy(msg_buffer.msg + msg_buffer.msg_len, msg, msg_len + ZERO_BYTE);
    msg_buffer.msg_len += msg_len;
    return true;
}

/**
 * @brief Frees the memory used by the buffer and resets its state.
 * 
 * @note This functions was created using ChatGPT
 */
void buffer_destroy(void) {
    if (msg_buffer.msg) {
        free(msg_buffer.msg);
        msg_buffer.msg = NULL;
        msg_buffer.msg_len = 0;
        msg_buffer.size = 0;
    }
}

/**
 * @brief Extracts a complete message from the buffer, if available.
 *
 * A message is considered complete if it ends with "\r\n".
 *
 * @return Dynamically allocated message string (caller must free), or NULL if incomplete.
 * 
 * @note This functions was created using ChatGPT
 */
char *buffer_get_msg(void) {
    if (buffer_is_empty()) {
        return NULL;
    }
    char *end = crlf();
    if (!end) {
        return NULL;
    }
    int msg_len = end - msg_buffer.msg + 2;
    char *msg = malloc(msg_len + 1);
    if (!msg) {
        return NULL;
    }
    memcpy(msg, msg_buffer.msg, msg_len);
    msg[msg_len] = '\0';
    memmove(msg_buffer.msg, msg_buffer.msg + msg_len, msg_buffer.msg_len - msg_len);
    msg_buffer.msg_len -= msg_len;
    return msg;
}

/**
 * @brief Checks if the message buffer is currently empty.
 *
 * @return true if empty, false otherwise.
 * 
 * @note This functions was created using ChatGPT
 */
bool buffer_is_empty(void) {
    return msg_buffer.msg_len == 0;
}
