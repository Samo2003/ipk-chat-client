#ifndef QUEUE_H
#define QUEUE_H

#include "common.h"

#define MAX_QUEUE_SIZE 16

typedef struct msg {
    char *msg;
    int msg_len;
} msg_t;

typedef struct msg_queue {
    msg_t msgs[MAX_QUEUE_SIZE];
    int front;
    int end;
    int size;
} msg_queue_t;

extern msg_queue_t msg_queue;

bool queue_is_empty(void);
bool queue_is_full(void);
bool queue_add(char *msg, int msg_len);
bool queue_front(char **msg, int *msg_len);
void queue_destroy(void);

#endif
