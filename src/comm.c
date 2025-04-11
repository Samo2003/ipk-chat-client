#include "../lib/comm.h"

comm_t *get_comm(void) {
    static comm_t comm = {0};
    if (parameters.tcp) {
        comm.setup = tcp_setup;
        comm.clean_up = tcp_clean_up;
        comm.send_msg = tcp_send_msg;
        comm.recv_msg = tcp_recv_msg;
    } else {
        comm.setup = udp_setup;
        comm.clean_up = udp_clean_up;
        comm.send_msg = udp_send_msg;
        comm.recv_msg = udp_recv_msg;
    }
    comm.socket = comm.setup();
    if (comm.socket == -1) {
        return NULL;
    }
    comm.processing = false;
    comm.incomplete = false;
    return &comm;
}

void clean_up_comm(void) {
    comm->clean_up(comm->socket);
    freeaddrinfo(client.res);
    close(comm->socket);
    list_destroy();
    queue_destroy();
    buffer_destroy();
    tcflush(STDIN_FILENO, TCIFLUSH);
}
