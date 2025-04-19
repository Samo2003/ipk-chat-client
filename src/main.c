/**
 * @file main.c
 * @brief Entry point for the chat client application.
 *
 * This file initializes global components, handles interrupts,
 * processes command-line arguments, and starts the finite state machine (FSM).
 * 
 * @author Samuel Stefanik (xstefas00)
 * @date 2025-04-19
 */
#include "../lib/fsm.h"

parameters_t parameters = {0};     /**< Global parameters parsed from command-line arguments. */
comm_t *comm;                      /**< Pointer to communication handler for TCP/UDP abstraction. */
client_t client = {0};             /**< Client-specific data. */
volatile int interrupt = 0;        /**< Interrupt flag used to handle Ctrl+C (SIGINT) cleanly. */

/**
 * @brief Signal handler for SIGINT (Ctrl+C).
 * 
 * Sets the global interrupt flag to indicate the program should terminate gracefully.
 * 
 * @param sig Signal number (unused).
 */
void interrupt_handle(int sig) {
    (void)sig;
    interrupt = 1;
}

/**
 * @brief Main function of the client.
 * 
 * Initializes the program, processes command-line arguments, sets up communication,
 * and enters the main finite state machine loop.
 * 
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line argument strings.
 * @return EXIT_SUCCESS on successful completion, EXIT_FAILURE otherwise.
 */
int main(int argc, char **argv) {
    signal(SIGINT, interrupt_handle);
    if (process_args(argc, argv) != EXIT_SUCCESS) {
        return EXIT_FAILURE;
    }
    if ((comm = get_comm()) == NULL) {
        return EXIT_FAILURE;
    }
    return fsm();
}
