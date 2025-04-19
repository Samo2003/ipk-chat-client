/**
 * @file queue.h
 * @brief Definition of a simple circular message queue.
 *
 * This module provides a fixed-size FIFO queue implementation for storing
 * dynamically allocated messages. Used for buffering outgoing messages in a chat client/server.
 * 
 * @author Samuel Stefanik (xstefas00)
 * @date 2025-04-19
 */
#ifndef QUEUE_H
#define QUEUE_H

#include "common.h"

#define MAX_QUEUE_SIZE 16   /**< Maximum number of messages the queue can hold. */

/**
 * @brief Represents a single message in the queue.
 */
typedef struct msg {
    char *msg;     /**< Pointer to dynamically allocated message string. */
    int msg_len;   /**< Length of the message. */
} msg_t;

/**
 * @brief Represents the message queue structure.
 */
typedef struct msg_queue {
    msg_t msgs[MAX_QUEUE_SIZE];  /**< Fixed-size array of message slots. */
    int front;                   /**< Index of the front message. */
    int end;                     /**< Index where the next message will be added. */
    int size;                    /**< Current number of messages in the queue. */
} msg_queue_t;

// Global instance of the message queue.
extern msg_queue_t msg_queue;

/**
 * @brief Checks if the message queue is empty.
 * 
 * @return true if the queue is empty, false otherwise.
 * 
 * @note This functions was created using ChatGPT
 */
bool queue_is_empty(void);

/**
 * @brief Checks if the message queue is full.
 * 
 * @return true if the queue is full, false otherwise.
 * 
 * @note This functions was created using ChatGPT
 */
bool queue_is_full(void);

/**
 * @brief Adds a new message to the end of the queue.
 * 
 * The message is copied into dynamically allocated memory.
 * 
 * @param msg Pointer to the message data.
 * @param msg_len Length of the message in bytes.
 * @return true if the message was successfully added, false on failure or if full.
 * 
 * @note This functions was created using ChatGPT
 */
bool queue_add(char *msg, int msg_len);

/**
 * @brief Retrieves the message at the front of the queue.
 * 
 * The message is removed from the queue and ownership is transferred to the caller.
 * The caller is responsible for freeing the message buffer.
 * 
 * @param msg Pointer to store the address of the message.
 * @param msg_len Pointer to store the length of the message.
 * @return true if a message was retrieved, false if the queue is empty.
 * 
 * @note This functions was created using ChatGPT
 */
bool queue_front(char **msg, int *msg_len);

/**
 * @brief Frees all messages currently in the queue and resets the queue state.
 * 
 * @note This functions was created using ChatGPT
 */
void queue_destroy(void);

#endif
