#include "../lib/args.h"

parameters_t parameters = {0};

int main(int argc, char **argv) {
    if (process_args(argc, argv) != EXIT_SUCCESS) {
        return EXIT_FAILURE;
    }
    return 0;
}
