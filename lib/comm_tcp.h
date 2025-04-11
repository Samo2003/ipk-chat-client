#ifndef COMM_TCP_H
#define COMM_TCP_H

#include "args.h"
#include "buffer.h"
#include "tcp_msg.h"

int tcp_setup(void);
void tcp_clean_up(int sock_fd);
int tcp_send_msg(msg_type_t type, int sock_fd);
msg_type_t tcp_recv_msg(int sock_fd);
msg_type_t tcp_process_msg(char *buffer);

#endif
