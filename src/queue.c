/**
 * @file queue.c
 * @brief Implementation of a simple circular message queue.
 * 
 * @author Samuel Stefanik (xstefas00)
 * @date 2025-04-19
 */
#include "../lib/queue.h"

// Global instance of the message queue.
msg_queue_t msg_queue = {0};

/**
 * @brief Checks if the message queue is empty.
 * 
 * @return true if the queue is empty, false otherwise.
 * 
 * @note This functions was created using ChatGPT
 */
bool queue_is_empty(void) {
    return msg_queue.size == 0;
}

/**
 * @brief Checks if the message queue is full.
 * 
 * @return true if the queue is full, false otherwise.
 * 
 * @note This functions was created using ChatGPT
 */
bool queue_is_full(void) {
    return msg_queue.size == MAX_QUEUE_SIZE;
}

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
bool queue_add(char *msg, int msg_len) {
    if (queue_is_full()) {
        return false;
    }
    char *copy = malloc(msg_len);
    if (!copy) {
        return false;
    }
    memcpy(copy, msg, msg_len);
    msg_queue.msgs[msg_queue.end].msg = copy;
    msg_queue.msgs[msg_queue.end].msg_len = msg_len;

    msg_queue.end = (msg_queue.end + 1) % MAX_QUEUE_SIZE;
    msg_queue.size++;
    return true;
}

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
bool queue_front(char **msg, int *msg_len) {
    if (queue_is_empty()) {
        return false;
    }
    *msg = msg_queue.msgs[msg_queue.front].msg;
    *msg_len = msg_queue.msgs[msg_queue.front].msg_len;
    msg_queue.front = (msg_queue.front + 1) % MAX_QUEUE_SIZE;
    msg_queue.size--;
    return true;
}

/**
 * @brief Frees all messages currently in the queue and resets the queue state.
 * 
 * @note This functions was created using ChatGPT
 */
void queue_destroy(void) {
    while (!queue_is_empty()) {
        char *msg;
        int msg_len;
        queue_front(&msg, &msg_len);
        free(msg);
    }
}
