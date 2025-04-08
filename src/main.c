#include "../lib/fsm.h"

parameters_t parameters = {0};
comm_t *comm;
client_t client = {0};
volatile sig_atomic_t interrupt = 0;

void interrupt_handle(int sig) {
    (void)sig;
    interrupt = 1;
}

int main(int argc, char **argv) {
    signal(SIGINT, interrupt_handle);
    if (process_args(argc, argv) != EXIT_SUCCESS) {
        return EXIT_FAILURE;
    }
    comm = get_comm();
    if (comm == NULL) {
        return EXIT_FAILURE;
    }
    return fsm_start();
}
