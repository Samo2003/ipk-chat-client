#include "../lib/comm_tcp.h"

const tcp_msg_temp_t msg_temp[] = {
    {REPLY, "REPLY OK IS %s\r\n", 7, parse_reply},
    {NREPLY, "REPLY NOK IS %s\r\n", 7, parse_nreply},
    {AUTH, "AUTH %s AS %s USING %s\r\n", 4, parse_unexpected},
    {JOIN, "JOIN %s AS %s\r\n", 4, parse_unexpected},
    {MSG, "MSG FROM %s IS %s\r\n", 3, parse_msg},
    {ERR, "ERR FROM %s IS %s\r\n", 3, parse_err},
    {BYE, "BYE FROM %s\r\n", 3, parse_bye}
};

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

void tcp_clean_up(int sock_fd) {
    shutdown(sock_fd, SHUT_RD);
    shutdown(sock_fd, SHUT_WR);
    shutdown(sock_fd, SHUT_RDWR);
}

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

msg_type_t tcp_process_msg(char *buffer) {
    for (int i = 0; i < TEMP_COUNT; i++) {
        if (strncasecmp(buffer, msg_temp[i].temp, msg_temp[i].len) == 0) {
            return msg_temp[i].parse_func(buffer);
        }
    }
    fprintf(stdout, "ERROR: Unknown massage received\n");
    return ERROR;
}
