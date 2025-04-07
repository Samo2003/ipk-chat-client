#include "../lib/prompt.h"

prompt_t await(void) {
    fd_set fd_descs;
    prompt_t prompt = {0};
    prompt.input_type = FAULT;
    
    struct timeval timeout = {.tv_sec = 5, .tv_usec = 0};
    while(!proccesing && interrupt == 0) {
        FD_ZERO(&fd_descs);
        FD_SET(comm->socket, &fd_descs);
        if (!proccesing) {
            FD_SET(STDIN_FILENO, &fd_descs);
        }

        int fd_count = select(FD_SETSIZE, &fd_descs, NULL, NULL, &timeout);
        if (fd_count == -1) {
            perror("ERROR: select");
            return prompt;
        }
        if (FD_ISSET(comm->socket, &fd_descs)) {
            prompt.input_type = SERVER;
            prompt.type = comm->recv_msg(comm->socket);
            return prompt;
        }
        if (!proccesing && FD_ISSET(STDIN_FILENO, &fd_descs)) {
            prompt.input_type = USER;
            prompt.type = parse_user_input();
            return prompt;
        }
    }
    return prompt;
}
