#ifndef COMM_H
#define COMM_H

#include "comm_tcp.h"
#include "comm_udp.h"
#include "args.h"
#include <termios.h>

typedef struct comm {
    int (*setup)(void);
    void (*clean_up)(int sock_fd);
    int (*send_msg)(msg_type_t type, int sock_fd);
    msg_type_t (*recv_msg)(int sock_fd);
    bool (*check_previous_msg)(msg_type_t *type);
    int socket;
    bool processing;
    bool incomplete;
} comm_t;

extern comm_t *comm;

#define SEND(type)                                              \
    if (comm->send_msg(type, comm->socket) != EXIT_SUCCESS) {   \
        fprintf(stdout, "ERROR: Failed to send message\n");     \
        return EXIT_FAILURE;                                    \
    }

comm_t *get_comm(void);

void clean_up_comm(void);


#endif
