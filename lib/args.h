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

#define CHECK_OPTARG_AND_REDEF(condition)                           \
    if (optarg == NULL || optarg[0] == '-') {                       \
        fprintf(stderr, "ERROR: missing -%c parameter\n", optopt);  \
        return EXIT_FAILURE;                                        \
    } else if (condition) {                                         \
        fprintf(stderr, "ERROR: redefinition of -%c\n", optopt);    \
        return EXIT_FAILURE;                                        \
    }

#define PARSE_NUMBER(dst, type, max)\
    if (!parse_int(max, &result)) {\
        return EXIT_FAILURE;\
    }\
    dst = (type)result;\
    only_opt = false;

int process_args(int argc, char **argv);

#endif
