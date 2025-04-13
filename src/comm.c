#include "../lib/comm.h"

static bool tcp_check_previous(msg_type_t *type) {
    char *msg;
    if ((msg = buffer_get_msg())) {
        *type = tcp_process_msg(msg);
        free(msg);
        return true;
    }
    comm->incomplete = !buffer_is_empty();
    return false;
}

static bool udp_check_previous(msg_type_t *type) {
    int len;
    char *msg;
    if (!queue_is_empty()) {
        queue_front(&msg, &len);
        *type = udp_process_msg(msg, len, false);
        free(msg);
        return true;
    }
    return false;
}

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
