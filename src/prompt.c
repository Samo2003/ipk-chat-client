/**
 * @file prompt.c
 * @brief Handles waiting for and processing user or server input.
 * 
 * @author Samuel Stefanik (xstefas00)
 * @date 2025-04-19
 */
#include "../lib/prompt.h"

/**
 * @brief Calculates the remaining time for a server response.
 *
 * Given the start time of the operation, this function computes how much time
 * remains within a fixed 4-second timeout window. It accounts for microsecond precision.
 *
 * @param start Pointer to the `timeval` struct holding the start time.
 * @param remaining Pointer to a `timeval` struct to store the computed remaining time.
 * 
 * @note This functions was created using ChatGPT
 */
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

/**
 * @brief Waits for and retrieves the next user or server input.
 *
 * This function blocks using `select()` until input is ready from either:
 *  - the server socket (if a message is expected),
 *  - or standard input (if in idle state).
 * 
 * If a message is already buffered (from a previous server response), it is returned immediately.
 * If a timeout occurs when waiting for a required server response, an error prompt is returned.
 *
 * @return A `prompt_t` struct containing the input type (USER or SERVER) and message type.
 */
prompt_t await(void) {
    fd_set fd_descs;
    prompt_t prompt = {0};
    prompt.input_type = SERVER;
    int fd_count;
    
    // Check if there's a previous buffered message.
    if (comm->check_previous_msg(&prompt.type) && prompt.type != PING) {
        return prompt;
    }
    bool incomplete_tcp_message = !buffer_is_empty();
    
    struct timeval start_time, remaining_time, *ptr = NULL;

    // Enable timeout if awaiting server response.
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
        if (!comm->processing && !incomplete_tcp_message) {
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
            if (prompt.type == PING) {
                continue;
            }
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
