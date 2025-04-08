#include "../lib/prompt.h"

static void calculate_remaining_time(struct timeval *start, struct timeval *remaining) {
    struct timeval now;
    gettimeofday(&now, NULL);

    time_t elapsed_sec = now.tv_sec - start->tv_sec;
    suseconds_t elapsed_usec = now.tv_usec - start->tv_usec;
    
    if (elapsed_usec < 0) {
        elapsed_usec += 1000000;
        elapsed_sec--;
    }

    remaining->tv_sec = 4 - elapsed_sec;
    remaining->tv_usec = 1000000 - elapsed_usec;
    if (remaining->tv_usec == 1000000) {
        remaining->tv_usec = 0;
        remaining->tv_sec += 1;
    }
}

prompt_t await(void) {
    fd_set fd_descs;
    prompt_t prompt = {0};
    prompt.input_type = SERVER;
    int fd_count;
    
    struct timeval start_time, remaining_time, *ptr = NULL;

    if (comm->processing) {
        gettimeofday(&start_time, NULL);
        ptr = &remaining_time;
    }

    while(true) {
        if (comm->processing) {
            calculate_remaining_time(&start_time, &remaining_time);
        }
        
        FD_ZERO(&fd_descs);
        FD_SET(comm->socket, &fd_descs);
        if (!comm->processing) {
            FD_SET(STDIN_FILENO, &fd_descs);
        }

        fd_count = select(FD_SETSIZE, &fd_descs, NULL, NULL, ptr);

        if (fd_count == -1) {
            if (errno == EINTR) {
                if (!comm->processing) {
                    prompt.type = LOCAL;
                    return prompt;
                }
                continue;
            } else {
                perror("ERROR: select");
                return prompt;
            }
        }
        if (fd_count == 0 && comm->processing) {
            fprintf(stdout, "ERROR: Server is not responding\n");
            prompt.type = ERROR;
            return prompt;
        }
        if (FD_ISSET(comm->socket, &fd_descs)) {
            prompt.type = comm->recv_msg(comm->socket);
            return prompt;
        }
        if (!comm->processing && FD_ISSET(STDIN_FILENO, &fd_descs)) {
            prompt.input_type = USER;
            prompt.type = parse_user_input();
            return prompt;
        }
    }

    return prompt;
}
