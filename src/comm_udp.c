/**
 * @file comm_udp.c
 * @brief UDP communication backend implementation.
 *
 * This file implements the UDP-specific functions required by the communication
 * abstraction layer. It includes functions for message sending, receiving, setup,
 * and response handling using the custom protocol.
 * 
 * Messages are sent using `sendto`, and responses are handled with timeouts and 
 * retransmissions. A basic reliability mechanism is implemented through message
 * acknowledgment (CONFIRM messages). Incoming messages are matched to handlers
 * defined in the `msg` table.
 * 
 * @author Samuel Stefanik (xstefas00)
 * @date 2025-04-19
 */
#include "../lib/comm_udp.h"

const udp_msg_t msg[] = {
    {CONFIRM, udp_parse_confirm},
    {REPLY, udp_parse_reply},
    {AUTH, udp_parse_unexpected},
    {JOIN, udp_parse_unexpected},
    {MSG, udp_parse_msg},
    {ERR, udp_parse_err},
    {BYE, udp_parse_bye},
    {PING, udp_parse_ping}
};

/**
 * @brief Sends a CONFIRM message to acknowledge a received message.
 *
 * @param sock_fd Socket file descriptor.
 * @param msg_id Message ID to confirm.
 * @return EXIT_SUCCESS on success, EXIT_FAILURE on error.
 */
static int send_confirm(int sock_fd, uint16_t msg_id) {
    char *confirm = build_header(CONFIRM, msg_id, 0);
    PTR_CHECK(confirm)
    if (sendto(sock_fd, confirm, HEADER_SIZE, 0, client.res->ai_addr, client.res->ai_addrlen) != HEADER_SIZE) {
        perror("ERROR: sendto");
        free(confirm);
        return EXIT_FAILURE;
    }
    free(confirm);
    return EXIT_SUCCESS;
}

/**
 * @brief Initializes and configures a UDP socket.
 *
 * Sets up the socket, applies a receive timeout, and resolves the server address.
 * 
 * @return Socket file descriptor on success, -1 on error.
 */
int udp_setup(void) {
    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd <= 0) {
        perror("ERROR: UDP socket");
        return -1;
    }

    struct timeval timeout = {.tv_sec = parameters.timeout / 1000, .tv_usec = (parameters.timeout % 1000) * 1000};
    if (setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) == -1) {
        perror("ERROR: setsockopt");
        close(sock_fd);
        return -1;
    }

    struct addrinfo hints = {0};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    if (getaddrinfo(parameters.ip_or_domain, parameters.port, &hints, &client.res) != 0) {
        perror("ERROR: getaddrinfo");
        close(sock_fd);
        return -1;
    }
    return sock_fd;
}

/**
 * @brief Cleans up UDP communication. (No-op for UDP.)
 *
 * UDP does not maintain a connection, so this function does nothing.
 *
 * @param sock_fd Socket file descriptor (unused).
 */
void udp_clean_up(int sock_fd) {
    (void)sock_fd;
    return;
}

/**
 * @brief Sends a message via UDP and handles response.
 *
 * Implements retransmission and simple reliability via CONFIRM messages.
 * If a different message is received instead of CONFIRM, it is processed
 * and possibly stored.
 *
 * @param type Type of the message to send.
 * @param sock_fd Socket file descriptor.
 * @return EXIT_SUCCESS if message is acknowledged, EXIT_FAILURE otherwise.
 */
int udp_send_msg(msg_type_t type, int sock_fd) {
    int len = 0;
    char *msg = udp_build_msg(type, &len);
    bool resend = true;
    PTR_CHECK(msg);
    for (int i = 0; i <= parameters.retransmissions; i++) {
        if (resend) {
            if (sendto(sock_fd, msg, len, 0, client.res->ai_addr, client.res->ai_addrlen) != len) {
                perror("ERROR: sendto");
                free(msg);
                return EXIT_FAILURE;
            }
        }
        resend = true;
        char buffer[BUFFER_SIZE] = {'0'};

        int b_rcv = recvfrom(sock_fd, buffer, BUFFER_SIZE, 0, client.res->ai_addr, &client.res->ai_addrlen);
        if (b_rcv > 0) {
            if (b_rcv == 3 && buffer[0] == CONFIRM && client.msg_id == to_uint16(&buffer[1])) {
                client.msg_id++;
                free(msg);
                return EXIT_SUCCESS;
            } else {
                if (buffer[0] != CONFIRM && send_confirm(sock_fd, to_uint16(&buffer[1])) != EXIT_SUCCESS) {
                    return EXIT_FAILURE;
                }
                if (!list_contains(to_uint16(&buffer[1]))) {
                    if (!queue_add(buffer, b_rcv) || list_append(to_uint16(&buffer[1])) != EXIT_SUCCESS) {
                        return EXIT_FAILURE;
                    }
                }
                i--;
                resend = false;
            }
        } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
            continue;
        } else {
            perror("ERROR: recvfrom");
            free(msg);
            return EXIT_FAILURE;
        }
    }

    free(msg);
    return EXIT_FAILURE;
}

/**
 * @brief Receives a UDP message and processes it.
 *
 * If a non-CONFIRM message is received, it sends back a CONFIRM.
 * 
 * @param sock_fd Socket file descriptor.
 * @return Message type on success, ERROR on failure.
 */
msg_type_t udp_recv_msg(int sock_fd) {
    char buffer[BUFFER_SIZE] = {'\0'};
    int b_rcv = recvfrom(sock_fd, buffer, BUFFER_SIZE, 0, client.res->ai_addr, &client.res->ai_addrlen);
    if (b_rcv >= HEADER_SIZE) {
        if (buffer[0] != CONFIRM && send_confirm(sock_fd, to_uint16(&buffer[1])) != EXIT_SUCCESS) {
            return ERROR;
        }
        return udp_process_msg(buffer, b_rcv, true);
    }
    fprintf(stdout, "ERROR: recvfrom fault\n");
    return ERROR;
}

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
msg_type_t udp_process_msg(char *buffer, int msg_len, bool store) {
    for (int i = 0; i < MSG_COUNT; i++) {
        if ((uint8_t)buffer[0] == msg[i].type) {
            if (store) {
                if (list_contains(to_uint16(&buffer[1]))) {
                    return LOCAL;
                }
                if (list_append(to_uint16(&buffer[1])) != EXIT_SUCCESS) {
                    return ERROR;
                }
            }
            return msg[i].parse_func(buffer, msg_len);
        }
    }
    fprintf(stdout, "ERROR: Unknown message\n");
    return ERROR;
}

/**
 * @brief Checks the queue for previously received but unprocessed messages.
 *
 * If a message is found, it is processed and the result is returned via `type`.
 * 
 * @param type Pointer to store the resulting message type.
 * @return true if a message was processed, false if the queue is empty.
 */
bool udp_check_previous(msg_type_t *type) {
    int len;
    char *msg;
    if (!queue_is_empty()) {
        queue_front(&msg, &len);
        *type = udp_process_msg(msg, len, false);
        free(msg);
        return true;
    }
    return false;
}
