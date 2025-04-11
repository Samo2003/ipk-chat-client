#ifndef LIST_H
#define LIST_H

#include "common.h"

#define LIST_LEN 20

typedef struct list {
    int count;
    int capacity;
    uint16_t *content;
} list_t;

extern list_t msg_id_list;

bool list_contains(uint16_t number);
int list_append(uint16_t number);
void list_destroy(void);

#endif
