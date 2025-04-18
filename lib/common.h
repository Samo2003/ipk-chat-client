#ifndef COMMON_H
#define COMMON_H

#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdint.h>
#include <errno.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ctype.h>
#include <signal.h>

#define MAX_NAME_SIZE 20
#define MAX_SECRET_SIZE 128
#define MAX_MSG_SIZE 60000
#define ZERO_BYTE 1
#define BUFFER_SIZE UINT16_MAX

typedef enum msg_type{
    CONFIRM = 0x00,
    REPLY,
    AUTH,
    JOIN,
    MSG,
    NREPLY,
    ERROR,
    LOCAL,
    PING = 0xFD,
    ERR,
    BYE
} msg_type_t;

typedef struct client {
    uint16_t msg_id;
    char username[MAX_NAME_SIZE + ZERO_BYTE];
    char channel_ID[MAX_NAME_SIZE + ZERO_BYTE];
    char secret[MAX_SECRET_SIZE + ZERO_BYTE];
    char display_name[MAX_NAME_SIZE + ZERO_BYTE];
    char msg_content[MAX_MSG_SIZE + ZERO_BYTE];
    struct addrinfo *res;
} client_t;

extern client_t client;

#define PTR_CHECK(ptr)                                          \
    if (!ptr) {                                                 \
        fprintf(stderr, "ERROR: using NULL pointer\n");         \
        return EXIT_FAILURE;                                    \
    }

bool is_valid_alphanum_underscore(char *str, int max_len);
bool is_valid_printable(char *str, int max_len);
bool is_valid_msg_content(char *str, int max_len);
void copy(char *dest, char *src, int size);

#endif
