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

#define BUFFER_SIZE 1500
#define MAX_NAME_SIZE 20
#define MAX_SECRET_SIZE 128
#define MAX_MSG_SIZE 60000
#define ZERO_BYTE 1

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


bool is_valid_alphanum_underscore(char *str, size_t max_len);
bool is_valid_printable(char *str, size_t max_len);
bool is_valid_msg_content(char *str, size_t max_len);
void copy(char *dest, char *src, size_t size);

#endif
