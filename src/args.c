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

int process_args(int argc, char **argv) {
    parameters_setup();
    int opt;

    bool protocol_set = false, timeout_set = false, retransmissions_set = false, help_set = false, only_opt = true;

    while((opt = getopt(argc, argv, ":t:s:p:d:r:h")) != -1) {
        switch(opt) {
            case 't':
                if (optarg == NULL || optarg[0] == '-') {
                    fprintf(stderr, "ERROR: missing protocol parameter\n");
                    return EXIT_FAILURE;
                } else if (protocol_set) {
                    fprintf(stderr, "ERROR: redefinition of protocol\n");
                    return EXIT_FAILURE;
                } else {
                    if (strcmp(optarg, "tcp") == 0) {
                        parameters.tcp = true;
                    } else if (strcmp(optarg, "udp") == 0) {
                        parameters.tcp = false;
                    } else {
                        fprintf(stderr, "ERROR: invalid protocol\n");
                        return EXIT_FAILURE;
                    }
                    protocol_set = true;
                    only_opt = false;
                }
                break;
            case 's':
                if (optarg == NULL || optarg[0] == '-') {
                    fprintf(stderr, "ERROR: missing ip/domain name parameter\n");
                    return EXIT_FAILURE;
                } else if (parameters.ip_or_domain != NULL) {
                    fprintf(stderr, "ERROR: redefinition of ip/domain name\n");
                    return EXIT_FAILURE;
                } else {
                    parameters.ip_or_domain = optarg;
                    only_opt = false;
                }
                break;
            case 'p':
                if (optarg == NULL || optarg[0] == '-') {
                    fprintf(stderr, "ERROR: missing port parameter\n");
                    return EXIT_FAILURE;
                } else if (parameters.port != NULL) {
                    fprintf(stderr, "ERROR: redefinition of port\n");
                    return EXIT_FAILURE;
                } else {
                    free(parameters.port);
                    parameters.port = optarg;
                    only_opt = false;
                }
                break;
            case 'd':
                if (optarg == NULL || optarg[0] == '-') {
                    fprintf(stderr, "ERROR: missing timeout parameter\n");
                    return EXIT_FAILURE;
                } else if (timeout_set) {
                    fprintf(stderr, "ERROR: redefinition of timeout\n");
                    return EXIT_FAILURE;
                } else {
                    char *endptr;
                    long timeout = strtol(optarg, &endptr, 10);
                    if (errno != 0 || *endptr != '\0' || timeout < 0 || timeout > UINT16_MAX) {
                        fprintf(stderr, "ERROR: invalid timeout value\n");
                        return EXIT_FAILURE;
                    }

                    parameters.timeout = (uint16_t)timeout;
                    timeout_set = true;
                    only_opt = false;
                }
                break;
            case 'r':
                if (optarg == NULL || optarg[0] == '-') {
                    fprintf(stderr, "ERROR: missing retransmissions parameter\n");
                    return EXIT_FAILURE;
                } else if (retransmissions_set) {
                    fprintf(stderr, "ERROR: redefinition of retransmissions\n");
                    return EXIT_FAILURE;
                } else {
                    char *endptr;
                    long retransmissions = strtol(optarg, &endptr, 10);
                    if (errno != 0 || *endptr != '\0' || retransmissions < 0 || retransmissions > UINT8_MAX) {
                        fprintf(stderr, "ERROR: invalid retransmissions value\n");
                        return EXIT_FAILURE;
                    }
                    parameters.retransmissions = (uint8_t)retransmissions;
                    retransmissions_set = true;
                    only_opt = false;
                }
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
    return EXIT_SUCCESS;
}
