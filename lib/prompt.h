#ifndef PROMPT_H
#define PROMPT_H

#include "comm.h"
#include "input.h"

#include <sys/time.h>

typedef enum {
    USER,
    SERVER
} input_type_t;

typedef struct prompt {
    input_type_t input_type;
    msg_type_t type;
} prompt_t;

extern bool processing;

prompt_t await(void);

#endif
