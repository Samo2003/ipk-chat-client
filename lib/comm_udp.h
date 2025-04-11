#ifndef COMM_UDP_H
#define COMM_UDP_H

#include "udp_msg.h"
#include "args.h"
#include "list.h"
#include "queue.h"

#define MSG_COUNT 8

typedef msg_type_t (*udp_parse)(char *buffer, int msg_len);

typedef struct udp_msg{
    msg_type_t type;
    udp_parse parse_func;
} udp_msg_t;

extern const udp_msg_t msg[MSG_COUNT];

int udp_setup(void);
void udp_clean_up(int sock_fd);
int udp_send_msg(msg_type_t type, int sock_fd);
msg_type_t udp_recv_msg(int sock_fd);
msg_type_t udp_process_queued_msg(char *buffer, int msg_len);

#endif
