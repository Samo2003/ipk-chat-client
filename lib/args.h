#ifndef ARGS_H
#define ARGS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdint.h>
#include <errno.h>

typedef struct parameters {
    bool tcp;
    char *ip_or_domain;
    char *port;
    uint16_t timeout;
    uint8_t retransmissions;
} parameters_t;

extern parameters_t parameters;

int process_args(int argc, char **argv);

#endif
