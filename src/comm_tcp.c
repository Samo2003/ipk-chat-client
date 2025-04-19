/**
 * @file comm_tcp.c
 * @brief Implementation of TCP communication backend for client.
 *
 * Handles connection setup, sending and receiving messages, processing received messages,
 * and managing a receive buffer for TCP communication. Uses a message template table
 * to identify and parse incoming messages.
 *
 * This module is intended to be used with a modular communication interface
 * that supports both TCP and UDP backends.
 *
 * @author: Samuel Stefanik (xstefas00)
 * @date: 2025-04-19
 */
#include "../lib/comm_tcp.h"

// Template definitions for supported TCP messages and their parsing functions.
const tcp_msg_temp_t msg_temp[] = {
    {REPLY, "REPLY OK IS %s\r\n", 7, parse_reply},
    {NREPLY, "REPLY NOK IS %s\r\n", 7, parse_nreply},
    {AUTH, "AUTH %s AS %s USING %s\r\n", 4, parse_unexpected},
    {JOIN, "JOIN %s AS %s\r\n", 4, parse_unexpected},
    {MSG, "MSG FROM %s IS %s\r\n", 3, parse_msg},
    {ERR, "ERR FROM %s IS %s\r\n", 3, parse_err},
    {BYE, "BYE FROM %s\r\n", 3, parse_bye}
};

/**
 * @brief Establishes a TCP connection to the server.
 *
 * Creates a socket and connects to the server address provided in parameters.
 *
 * @return Socket file descriptor on success, -1 on failure.
 */
int tcp_setup(void) {
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd <= 0) {
        perror("ERROR: TCP socket");
        return -1;
    }

    struct addrinfo hints = {0};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(parameters.ip_or_domain, parameters.port, &hints, &client.res) != 0) {
        close(sock_fd);
        perror("ERROR: getaddrinfo");
        return -1;
    }
    if (connect(sock_fd, client.res->ai_addr, client.res->ai_addrlen) != 0) {
        perror("ERROR: connect");
        close(sock_fd);
        freeaddrinfo(client.res);
        return -1;
    }
    return sock_fd;
}

/**
 * @brief Cleanly shuts down the TCP socket.
 *
 * @param sock_fd The socket file descriptor to shut down.
 */
void tcp_clean_up(int sock_fd) {
    shutdown(sock_fd, SHUT_RD);
    shutdown(sock_fd, SHUT_WR);
    shutdown(sock_fd, SHUT_RDWR);
}

/**
 * @brief Sends a message of a given type over TCP.
 *
 * Builds the message using `tcp_build_msg` and sends it through the socket.
 *
 * @param type The message type to send.
 * @param sock_fd The socket file descriptor to send on.
 * @return EXIT_SUCCESS on success, EXIT_FAILURE on error.
 */
int tcp_send_msg(msg_type_t type, int sock_fd) {
    int len;
    char *msg = tcp_build_msg(type, &len);
    PTR_CHECK(msg)
    if (send(sock_fd, msg, len, 0) != len) {
        perror("ERROR: send");
        free(msg);
        return EXIT_FAILURE;
    }
    free(msg);
    return EXIT_SUCCESS;
}

/**
 * @brief Receives a message from the TCP socket.
 *
 * Appends incoming data to the receive buffer and attempts to extract a full message.
 * If a message is complete, it is parsed and returned.
 *
 * @param sock_fd The socket file descriptor to receive from.
 * @return Parsed message type on success, ERROR on failure, or LOCAL if message is incomplete.
 */
msg_type_t tcp_recv_msg(int sock_fd) {
    char buffer[BUFFER_SIZE] = {'\0'};
    size_t b_rcv = recv(sock_fd, buffer, BUFFER_SIZE - 1, 0);
    if (b_rcv > 0) {
        if (!buffer_append(buffer, b_rcv)) {
            fprintf(stdout, "ERROR: realloc failed in buffer\n");
            return ERROR;
        }
        char *msg = buffer_get_msg();
        if (msg) {
            msg_type_t ret = tcp_process_msg(msg);
            free(msg);
            return ret;
        }
        return LOCAL;
    }
    fprintf(stdout, "ERROR: recv fault\n");
    return ERROR;
}

/**
 * @brief Parses a TCP message using a matching message template.
 *
 * Loops through `msg_temp` and compares message prefix to determine the type.
 *
 * @param buffer The raw message string.
 * @return Parsed message type, or ERROR if unknown.
 */
msg_type_t tcp_process_msg(char *buffer) {
    for (int i = 0; i < TEMP_COUNT; i++) {
        if (strncasecmp(buffer, msg_temp[i].temp, msg_temp[i].len) == 0) {
            return msg_temp[i].parse_func(buffer);
        }
    }
    fprintf(stdout, "ERROR: Unknown massage received\n");
    return ERROR;
}

/**
 * @brief Checks if there are any previously received complete messages in the buffer.
 *
 * Used to handle messages received in a previous read call but not yet processed.
 *
 * @param type Output pointer to store parsed message type.
 * @return true if a message was found and parsed, false otherwise.
 */
bool tcp_check_previous(msg_type_t *type) {
    char *msg;
    if ((msg = buffer_get_msg())) {
        *type = tcp_process_msg(msg);
        free(msg);
        return true;
    }
    return false;
}
