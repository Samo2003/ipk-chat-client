#ifndef INPUT_H
#define INPUT_H

#include "common.h"

#define COMMAND_COUNT 4

typedef struct {
    char *cmd;
    msg_type_t (*parse)(char **args);
} command_t;

extern volatile int interrupt;

msg_type_t parse_user_input(void);

#endif
