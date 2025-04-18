#ifndef BUFFER_H
#define BUFFER_H

#include "common.h"

typedef struct msg_buffer_t {
    char *msg;
    int msg_len;
    int size;
} msg_buffer_t;

extern msg_buffer_t msg_buffer;

bool buffer_append(char *msg, int msg_len);
void buffer_destroy(void);
char *buffer_get_msg(void);
bool buffer_is_empty(void);

#endif
