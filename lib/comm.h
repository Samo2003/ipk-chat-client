#ifndef COMM_H
#define COMM_H

#include "comm_tcp.h"
#include "args.h"
#include <termios.h>

typedef struct comm {
    struct addrinfo *(*setup)(int *sock_fd);
    void (*clean_up)(int sock_fd);
    int (*send_msg)(msg_type_t type, int sock_fd);
    msg_type_t (*recv_msg)(int sock_fd);
    int socket;
    struct addrinfo *res;
    bool processing;
} comm_t;

extern comm_t *comm;

comm_t *get_comm(void);

void clean_up_comm(void);


#endif
