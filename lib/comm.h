/**
 * @file comm.h
 * @brief Communication abstraction layer for TCP and UDP.
 *
 * This header defines the `comm_t` structure and associated functions/macros
 * used to abstract communication logic between TCP and UDP implementations.
 * The communication interface is selected at runtime based on user parameters,
 * allowing the rest of the client code to remain protocol-agnostic.
 * 
 * The structure includes function pointers for setup, cleanup, message sending,
 * and receiving.
 * 
 * Also included are utility macros and external pointers for managing global
 * communication state.
 * 
 * @author Samuel Stefanik (xstefas00)
 * @date 2025-04-19
 */
#ifndef COMM_H
#define COMM_H

#include "comm_tcp.h"
#include "comm_udp.h"

/**
 * @brief Communication interface abstraction.
 *
 * Holds function pointers to the appropriate TCP or UDP implementations
 * for setting up the socket, sending and receiving messages, and cleanup.
 * Also stores the active socket file descriptor.
 */
typedef struct comm {
    int (*setup)(void);                             /**< Pointer to setup function (TCP or UDP) */
    void (*clean_up)(int sock_fd);                  /**< Pointer to cleanup function */
    int (*send_msg)(msg_type_t type, int sock_fd);  /**< Pointer to message sending function */
    msg_type_t (*recv_msg)(int sock_fd);            /**< Pointer to message receiving function */
    bool (*check_previous_msg)(msg_type_t *type);   /**< Pointer to previous message checker */
    int socket;                                     /**< Active socket file descriptor */
    bool processing;                                /**< Flag indicating message processing state */
} comm_t;

// Pointer to the global communication interface
extern comm_t *comm;

/**
 * @brief Sends a message using the active communication interface.
 *
 * This macro wraps the send_msg function and checks for errors.
 * On failure, prints an error message and returns EXIT_FAILURE.
 *
 * @param type The type of the message to be sent.
 */
#define SEND(type)                                              \
    if (comm->send_msg(type, comm->socket) != EXIT_SUCCESS) {   \
        fprintf(stdout, "ERROR: Failed to send message\n");     \
        return EXIT_FAILURE;                                    \
    }

/**
 * @brief Initializes and returns a pointer to the communication interface.
 *
 * Selects the appropriate communication backend (TCP or UDP) based on user parameters,
 * sets the function pointers accordingly, and calls the backend's setup function.
 * If setup fails, returns NULL. Otherwise, returns a pointer to the initialized
 * communication structure.
 *
 * @return Pointer to a statically allocated comm_t structure, or NULL on failure.
 */
comm_t *get_comm(void);

/**
 * @brief Cleans up resources related to communication and the client.
 *
 * Frees network resources, closes the socket, and destroys all shared
 * buffers, lists, and queues used by the client. Also flushes the terminal input.
 */
void clean_up_comm(void);

#endif
