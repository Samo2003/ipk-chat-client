/**
 * @file args.h
 * @brief Defines the structure and macros for handling command-line arguments.
 *
 * This module is responsible for parsing and validating the command-line arguments 
 * provided by the user when running the client application. It ensures that all 
 * required options are provided and checks for any conflicts or invalid arguments. 
 * The parsed values are stored in the global `parameters` structure for use throughout 
 * the client program.
 *
 * This module ensures that the user provides all necessary parameters, including 
 * the transport protocol (TCP or UDP), server IP or domain, port, and optional 
 * parameters such as timeout and retransmissions for UDP connections.
 *
 * @author Samuel Stefanik (xstefas00)
 * @date 2025-04-19
 */
#ifndef ARGS_H
#define ARGS_H

#include "common.h"

/**
 *  @brief Structure holding the command-line parameters.
 */
typedef struct parameters {
    bool tcp;                   /**< Transport protocol (TCP or UDP) */
    char *ip_or_domain;         /**< IP address or domain name of the server */
    char *port;                 /**< Server port as a string */
    uint16_t timeout;           /**< Timeout for UDP in milliseconds */
    uint8_t retransmissions;    /**< Maximum number of retransmissions for UDP */
} parameters_t;

// Global structure for the parsed parameters
extern parameters_t parameters;

/**
 * @brief Macro to check if an option argument is missing or redefined.
 */
#define CHECK_OPTARG_AND_REDEF(condition)                           \
    if (optarg == NULL || optarg[0] == '-') {                       \
        fprintf(stderr, "ERROR: missing -%c parameter\n", optopt);  \
        return EXIT_FAILURE;                                        \
    } else if (condition) {                                         \
        fprintf(stderr, "ERROR: redefinition of -%c\n", optopt);    \
        return EXIT_FAILURE;                                        \
    }

/**
 * @brief Macro to parse an integer argument and assign it to the destination variable.
 */
#define PARSE_NUMBER(dst, type, max)                                \
    if (!parse_int(max, &result)) {                                 \
        return EXIT_FAILURE;                                        \
    }                                                               \
    dst = (type)result;                                             \
    only_opt = false;

/**
 * @brief Parses CLI arguments and validates required ones.
 * 
 * This function parses the command-line arguments, checks for required options,
 * and ensures no conflicts or missing arguments. It stores the parsed data
 * in the `parameters` structure.
 * 
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return EXIT_SUCCESS or EXIT_FAILURE.
 */
int process_args(int argc, char **argv);

#endif
