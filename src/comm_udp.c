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

void udp_clean_up(int sock_fd) {
    (void)sock_fd;
    return;
}

int udp_send_msg(msg_type_t type, int sock_fd) {
    int len = 0;
    char *msg = udp_build_msg(type, &len);
    PTR_CHECK(msg);
    for (int i = 0; i <= parameters.retransmissions; i++) {
        if (sendto(sock_fd, msg, len, 0, client.res->ai_addr, client.res->ai_addrlen) != len) {
            perror("ERROR: sendto");
            free(msg);
            return EXIT_FAILURE;
        }
        char buffer[BUFFER_SIZE] = {'0'};

        int b_rcv = recvfrom(sock_fd, buffer, BUFFER_SIZE, 0, client.res->ai_addr, &client.res->ai_addrlen);
        if (b_rcv > 0) {
            if (b_rcv == 3 && buffer[0] == CONFIRM && client.msg_id == to_uint16(&buffer[1])) {
                client.msg_id++;
                free(msg);
                return EXIT_SUCCESS;
            } else {
                if (buffer[0] != CONFIRM) {
                    char *confirm = build_header(CONFIRM, to_uint16(&buffer[1]), 0);
                    if (sendto(sock_fd, confirm, HEADER_SIZE, 0, client.res->ai_addr, client.res->ai_addrlen) != HEADER_SIZE) {
                        perror("ERROR: sendto");
                        free(confirm);
                        return EXIT_FAILURE;
                    }
                    free(confirm);
                }
                if (!list_contains(to_uint16(&buffer[1]))) {
                    if (!queue_add(buffer, b_rcv) || list_append(to_uint16(&buffer[1])) != EXIT_SUCCESS) {
                        return EXIT_FAILURE;
                    }
                }
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

msg_type_t udp_recv_msg(int sock_fd) {
    char buffer[BUFFER_SIZE] = {'\0'};
    int b_rcv = recvfrom(sock_fd, buffer, BUFFER_SIZE, 0, client.res->ai_addr, &client.res->ai_addrlen);
    if (b_rcv >= HEADER_SIZE) {
        for (int i = 0; i < MSG_COUNT; i++) {
            if ((uint8_t)buffer[0] == msg[i].type) {
                if (msg[i].type != CONFIRM) {
                    char *confirm = build_header(CONFIRM, to_uint16(&buffer[1]), 0);
                    if (sendto(sock_fd, confirm, HEADER_SIZE, 0, client.res->ai_addr, client.res->ai_addrlen) != HEADER_SIZE) {
                        perror("ERROR: sendto");
                        free(confirm);
                        return EXIT_FAILURE;
                    }
                    free(confirm);
                    if (list_contains(to_uint16(&buffer[1]))) {
                        return LOCAL;
                    }
                    if (list_append(to_uint16(&buffer[1])) != EXIT_SUCCESS) {
                        return ERROR;
                    }
                }
                return msg[i].parse_func(buffer, b_rcv);
            }
        }
        fprintf(stdout, "ERROR: Unknown message\n");
        return ERROR;
    }
    fprintf(stdout, "ERROR: recvfrom fault\n");
    return ERROR;
}

msg_type_t udp_proccess_queued_msg(char *buffer, int msg_len) {
    for (int i = 0; i < MSG_COUNT; i++) {
        if ((uint8_t)buffer[0] == msg[i].type) {
            return msg[i].parse_func(buffer, msg_len);
        }
    }
    fprintf(stdout, "ERROR: Unknown message\n");
    return ERROR;
}
