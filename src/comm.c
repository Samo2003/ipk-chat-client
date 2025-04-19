/**
 * @file comm.c
 * @brief Implementation of communication backend initialization and cleanup.
 * 
 * @author Samuel Stefanik
 * @date 2025-04-19
 */
#include "../lib/comm.h"

/**
 * @brief Initializes and returns a pointer to the communication interface.
 *
 * @return Pointer to a statically allocated comm_t structure, or NULL on failure.
 */
comm_t *get_comm(void) {
    static comm_t comm = {0};
    if (parameters.tcp) {
        comm.setup = tcp_setup;
        comm.clean_up = tcp_clean_up;
        comm.send_msg = tcp_send_msg;
        comm.recv_msg = tcp_recv_msg;
        comm.check_previous_msg = tcp_check_previous;
    } else {
        comm.setup = udp_setup;
        comm.clean_up = udp_clean_up;
        comm.send_msg = udp_send_msg;
        comm.recv_msg = udp_recv_msg;
        comm.check_previous_msg = udp_check_previous;
    }
    comm.socket = comm.setup();
    if (comm.socket == -1) {
        return NULL;
    }
    comm.processing = false;
    return &comm;
}

/**
 * @brief Cleans up resources related to communication and the client.
 *
 * Frees network resources, closes the socket, and destroys all shared
 * buffers, lists, and queues used by the client. Also flushes the terminal input.
 */
void clean_up_comm(void) {
    comm->clean_up(comm->socket);
    freeaddrinfo(client.res);
    close(comm->socket);
    list_destroy();
    queue_destroy();
    buffer_destroy();
    tcflush(STDIN_FILENO, TCIFLUSH);
}
