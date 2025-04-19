/**
 * @file comm_udp.h
 * @brief UDP communication definitions and message parsing interface.
 *
 * This header defines the data structures and function prototypes used by the
 * UDP communication module. It includes a message dispatch table that maps
 * message types to their respective parsing functions.
 *
 * Used by the `comm_udp.c` implementation to support modular parsing
 * of different message types received over UDP.
 *
 * @author Samuel Stefanik (xstefas00)
 * @date 2025-04-19
 */
#ifndef COMM_UDP_H
#define COMM_UDP_H

#include "udp_msg.h"

#define MSG_COUNT 8     /**< Number of supported UDP message types. */

/**
 * @brief Function pointer type for parsing UDP messages.
 *
 * Each parser receives the raw message buffer and its length, and returns
 * the parsed message type.
 */
typedef msg_type_t (*udp_parse)(char *buffer, int msg_len);

/**
 * @brief Structure mapping a message type to its parser function.
 */
typedef struct udp_msg {
    msg_type_t type;           /**< Message type identifier. */
    udp_parse parse_func;      /**< Pointer to the parsing function. */
} udp_msg_t;

// Array of supported message types and their corresponding parsers.
extern const udp_msg_t msg[MSG_COUNT];

/**
 * @brief Initializes and configures a UDP socket.
 *
 * Sets up the socket, applies a receive timeout, and resolves the server address.
 * 
 * @return Socket file descriptor on success, -1 on error.
 */
int udp_setup(void);

/**
 * @brief Cleans up UDP communication. (No-op for UDP.)
 *
 * UDP does not maintain a connection, so this function does nothing.
 *
 * @param sock_fd Socket file descriptor (unused).
 */
void udp_clean_up(int sock_fd);

/**
 * @brief Sends a message via UDP and handles response.
 *
 * Implements retransmission and simple reliability via CONFIRM messages.
 * If a different message is received instead of CONFIRM, it is stored and possibly processes stored.
 *
 * @param type Type of the message to send.
 * @param sock_fd Socket file descriptor.
 * @return EXIT_SUCCESS if message is acknowledged, EXIT_FAILURE otherwise.
 */
int udp_send_msg(msg_type_t type, int sock_fd);

/**
 * @brief Receives a UDP message and processes it.
 *
 * If a non-CONFIRM message is received, it sends back a CONFIRM.
 * 
 * @param sock_fd Socket file descriptor.
 * @return Message type on success, ERROR on failure.
 */
msg_type_t udp_recv_msg(int sock_fd);

/**
 * @brief Processes a received UDP message.
 *
 * Matches the message type to its handler and optionally stores it
 * to avoid duplicates.
 *
 * @param buffer Raw message buffer.
 * @param msg_len Length of the message.
 * @param store Whether to store the message ID to prevent duplicates.
 * @return Parsed message type or ERROR.
 */
msg_type_t udp_process_msg(char *buffer, int msg_len, bool store);

/**
 * @brief Checks the queue for previously received but unprocessed messages.
 *
 * If a message is found, it is processed and the result is returned via `type`.
 * 
 * @param type Pointer to store the resulting message type.
 * @return true if a message was processed, false if the queue is empty.
 */
bool udp_check_previous(msg_type_t *type);

#endif
