#ifndef INPUT_H
#define INPUT_H

#include "common.h"

#define COMMAND_COUNT 4

typedef struct {
    char *cmd;
    msg_type_t (*parse)(char **args);
} command_t;

extern volatile sig_atomic_t interrupt;

bool is_valid_alphanum_underscore(char *str, size_t max_len);
bool is_valid_printable(char *str, size_t max_len);
bool is_valid_msg_content(char *str, size_t max_len);
void copy(char *dest, char *src, size_t size);
msg_type_t parse_user_input(void);

#endif
