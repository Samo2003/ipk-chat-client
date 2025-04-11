#ifndef TCP_MSG_H
#define TCP_MSG_H

#include "common.h"

#define TEMP_COUNT 7

// 1500 - IP and TCP header size
#define MAX_TCP_PACKET_SIZE 1460

typedef msg_type_t (*parse)(char *buffer);

typedef struct msg_temp{
    msg_type_t type;
    const char *temp;
    size_t len;
    parse parse_func;
} tcp_msg_temp_t;

extern const tcp_msg_temp_t msg_temp[TEMP_COUNT];

char *tcp_build_msg(msg_type_t type, int *msg_len);
msg_type_t parse_reply(char *buffer);
msg_type_t parse_nreply(char *buffer);
msg_type_t parse_unexpected(char *buffer);
msg_type_t parse_msg(char *buffer);
msg_type_t parse_err(char *buffer);
msg_type_t parse_bye(char *buffer);

#endif
