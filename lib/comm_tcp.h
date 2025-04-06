#ifndef COMM_TCP_H
#define COMM_TCP_H

#include "common.h"
#include "args.h"
#include "tcp_msg.h"

struct addrinfo *tcp_setup(int *sock_fd);
void tcp_clean_up(int sock_fd);
int tcp_send_msg(msg_type_t type, int sock_fd);
msg_type_t tcp_recv_msg(int sock_fd);

#endif
