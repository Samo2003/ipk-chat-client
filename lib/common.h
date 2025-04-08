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
    char username[21];
    char channel_ID[21];
    char secret[129];
    char display_name[21];
    char msg_content[60001];
} client_t;

extern client_t client;

#define PTR_CHECK(ptr)                                          \
    if (!ptr) {                                                 \
        fprintf(stderr, "ERROR: using NULL pointer\n");         \
        return EXIT_FAILURE;                                    \
    }

#define BUFFER_SIZE 1500

#endif
