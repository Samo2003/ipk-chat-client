#include "../lib/args.h"

static void parameters_setup(void) {
    parameters.port = malloc(sizeof(char) * 4);
    if (parameters.port == NULL) {
        fprintf(stderr, "ERROR: memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    strcpy(parameters.port, "4567");
    parameters.timeout = 250;
    parameters.retransmissions = 3;
}

static void print_help(void) {
    fprintf(stdout, "TODO\n");
    exit(0);
}

static bool set_protocol(void) {
    if (strcmp(optarg, "tcp") == 0) {
        parameters.tcp = true;
        return true;
    } else if (strcmp(optarg, "udp") == 0) {
        parameters.tcp = false;
        return true;
    } 
    fprintf(stderr, "ERROR: invalid protocol\n");
    return false;
}

bool parse_int(long max, long *out) {
    char *endptr;
    long val = strtol(optarg, &endptr, 10);
    if (errno != 0 || *endptr != '\0' || val < 0 || val > max) {
        fprintf(stderr, "ERROR: invalid -%c value\n", optopt);
        return false;
    }
    *out = val;
    return true;
}

int process_args(int argc, char **argv) {
    parameters_setup();
    int opt;
    long result;

    bool protocol_set = false, timeout_set = false, retransmissions_set = false, help_set = false, only_opt = true, port_set = false;

    while((opt = getopt(argc, argv, ":t:s:p:d:r:h")) != -1) {
        switch(opt) {
            case 't':
                CHECK_OPTARG_AND_REDEF(protocol_set)
                if (!set_protocol()) {
                    return EXIT_FAILURE;
                }
                protocol_set = true;
                only_opt = false;
                break;
            case 's':
                CHECK_OPTARG_AND_REDEF(parameters.ip_or_domain)
                parameters.ip_or_domain = optarg;
                only_opt = false;
                break;
            case 'p':
                CHECK_OPTARG_AND_REDEF(port_set)
                PARSE_NUMBER(result, long, UINT16_MAX)
                free(parameters.port);
                parameters.port = optarg;
                port_set = true;
                break;
            case 'd':
                CHECK_OPTARG_AND_REDEF(timeout_set)
                PARSE_NUMBER(parameters.timeout, uint16_t, UINT16_MAX)
                timeout_set = true;
                break;
            case 'r':
                CHECK_OPTARG_AND_REDEF(retransmissions_set)
                PARSE_NUMBER(parameters.retransmissions, uint8_t, UINT8_MAX)
                retransmissions_set = true;
                break;
            case 'h':
                if (help_set) {
                    fprintf(stderr, "ERROR: redefinition of help\n");
                    return EXIT_FAILURE;
                } else {
                    help_set = true;
                }
                break;
            case ':':
                fprintf(stderr, "ERROR: missing argument for option -%c\n", optopt);
                return EXIT_FAILURE;
            default:
                fprintf(stderr, "ERROR: invalid option -%c\n", optopt);
                return EXIT_FAILURE;
        }
    }
    if (only_opt && help_set) {
        print_help();
    }
    if (help_set) {
        fprintf(stderr, "ERROR: help option cannot be used with other options\n");
        return EXIT_FAILURE;
    }
    if (!protocol_set || parameters.ip_or_domain == NULL) {
        fprintf(stderr, "ERROR: missing required parameters\n");
        return EXIT_FAILURE;
    }
    if (optind != argc) {
        fprintf(stderr, "ERROR: unknown parameters after options\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
