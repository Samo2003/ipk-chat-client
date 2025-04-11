#include "../lib/prompt.h"

static void calculate_remaining_time(struct timeval *start, struct timeval *remaining) {
    struct timeval now;
    gettimeofday(&now, NULL);

    long elapsed_sec = now.tv_sec - start->tv_sec;
    long elapsed_usec = now.tv_usec - start->tv_usec;
    
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
    char *msg;
    
    struct timeval start_time, remaining_time, *ptr = NULL;

    if (!queue_is_empty()) {
        int len;
        queue_front(&msg, &len);
        prompt.type = udp_process_msg(msg, len, false);
        free(msg);
        return prompt;
    } else if ((msg = buffer_get_msg())) {
        prompt.type = tcp_process_msg(msg);
        free(msg);
        comm->incomplete = !buffer_is_empty();
        return prompt;
    } else {
        comm->incomplete = !buffer_is_empty();
    }

    bool require_response = comm->processing || comm->incomplete;

    if (require_response) {
        gettimeofday(&start_time, NULL);
        ptr = &remaining_time;
    }

    while(true) {
        if (require_response) {
            calculate_remaining_time(&start_time, &remaining_time);
        }
        
        FD_ZERO(&fd_descs);
        FD_SET(comm->socket, &fd_descs);
        if (!require_response) {
            FD_SET(STDIN_FILENO, &fd_descs);
        }

        fd_count = select(FD_SETSIZE, &fd_descs, NULL, NULL, ptr);

        if (fd_count == -1) {
            if (errno == EINTR) {
                if (!require_response) {
                    prompt.type = LOCAL;
                    return prompt;
                }
                continue;
            } else {
                perror("ERROR: select");
                return prompt;
            }
        }
        if (fd_count == 0 && require_response) {
            fprintf(stdout, "ERROR: Server is not responding\n");
            prompt.type = ERROR;
            return prompt;
        }
        if (FD_ISSET(comm->socket, &fd_descs)) {
            prompt.type = comm->recv_msg(comm->socket);
            if (prompt.type == PING) {
                continue;
            }
            return prompt;
        }
        if (!require_response && FD_ISSET(STDIN_FILENO, &fd_descs)) {
            prompt.input_type = USER;
            prompt.type = parse_user_input();
            return prompt;
        }
    }

    return prompt;
}
