/**
 * @file comm_tcp.h
 * @brief TCP communication interface for the client.
 *
 * Declares functions for setting up a TCP connection, sending and receiving messages,
 * and parsing received data using a template-based protocol.
 * This interface is part of a modular communication backend that supports TCP and UDP.
 *
 * @author: Samuel Stefanik (xstefas00)
 * @date: 2025-04-19
 */
#ifndef COMM_TCP_H
#define COMM_TCP_H

#include "tcp_msg.h"

/**
 * @brief Establishes a TCP connection to the server.
 *
 * Creates a socket and connects to the server address provided in parameters.
 *
 * @return Socket file descriptor on success, -1 on failure.
 */
int tcp_setup(void);

/**
 * @brief Cleanly shuts down the TCP socket.
 *
 * @param sock_fd The socket file descriptor to shut down.
 */
void tcp_clean_up(int sock_fd);

/**
 * @brief Sends a message of a given type over TCP.
 *
 * Builds the message using `tcp_build_msg` and sends it through the socket.
 *
 * @param type The message type to send.
 * @param sock_fd The socket file descriptor to send on.
 * @return EXIT_SUCCESS on success, EXIT_FAILURE on error.
 */
int tcp_send_msg(msg_type_t type, int sock_fd);

/**
 * @brief Receives a message from the TCP socket.
 *
 * Appends incoming data to the receive buffer and attempts to extract a full message.
 * If a message is complete, it is parsed and returned.
 *
 * @param sock_fd The socket file descriptor to receive from.
 * @return Parsed message type on success, ERROR on failure, or LOCAL if message is incomplete.
 */
msg_type_t tcp_recv_msg(int sock_fd);

/**
 * @brief Parses a TCP message using a matching message template.
 *
 * Loops through `msg_temp` and compares message prefix to determine the type.
 *
 * @param buffer The raw message string.
 * @return Parsed message type, or ERROR if unknown.
 */
msg_type_t tcp_process_msg(char *buffer);

/**
 * @brief Checks if there are any previously received complete messages in the buffer.
 *
 * Used to handle messages received in a previous read call but not yet processed.
 *
 * @param type Output pointer to store parsed message type.
 * @return true if a message was found and parsed, false otherwise.
 */
bool tcp_check_previous(msg_type_t *type);

#endif
