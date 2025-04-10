#ifndef UDP_MSG_H
#define UDP_MSG_H

#include "common.h"

#define HEADER_SIZE 3

char *build_header(msg_type_t type, uint16_t msg_id, int payload_len);
char *udp_build_msg(msg_type_t type, int *msg_len);
uint16_t to_uint16(char *buffer);
msg_type_t udp_parse_confirm(char *buffer, int msg_len);
msg_type_t udp_parse_reply(char *buffer, int msg_len);
msg_type_t udp_parse_unexpected(char *buffer, int msg_len);
msg_type_t udp_parse_msg(char *buffer, int msg_len);
msg_type_t udp_parse_err(char *buffer, int msg_len);
msg_type_t udp_parse_bye(char *buffer, int msg_len);
msg_type_t udp_parse_ping(char *buffer, int msg_len);

#endif
