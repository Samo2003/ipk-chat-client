#ifndef PROMPT_H
#define PROMPT_H

#include "comm.h"
#include "input.h"

typedef enum {
    USER,
    SERVER,
    FAULT
} input_type_t;

typedef struct prompt {
    input_type_t input_type;
    msg_type_t type;
} prompt_t;

extern bool proccesing;

prompt_t await(void);

#endif
