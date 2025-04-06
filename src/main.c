#include "../lib/comm.h"

parameters_t parameters = {0};
comm_t *comm;
client_t client = {0};

int main(int argc, char **argv) {
    if (process_args(argc, argv) != EXIT_SUCCESS) {
        return EXIT_FAILURE;
    }
    comm = get_comm();
    return 0;
}
