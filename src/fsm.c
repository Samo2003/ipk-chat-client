/**
 * @file fsm.c
 * @brief Finite State Machine (FSM) implementation for client-side protocol.
 * 
 * @author Samuel Stefanik (xstefas00)
 * @date 2025-04-19
 */
#include "../lib/fsm.h"

fsm_fn state = NULL;

/**
 * @brief Initial state of the client, waiting for AUTH command.
 * 
 * Handles user input to initiate authentication. If a valid AUTH message
 * is received, transitions to `state_auth`. Also handles unexpected server
 * messages by sending ERR and terminating on errors or BYE.
 */
int state_start(void) {
    prompt_t prompt = await();
    if (prompt.input_type == USER) {
        if (prompt.type == AUTH) {
            comm->processing = true;
            SEND(AUTH)
            state = state_auth;
        } else if (prompt.type == ERROR) {
            return EXIT_FAILURE;
        } else if (prompt.type != LOCAL) {
            fprintf(stdout, "ERROR: Authentication required\n");
        }
    } else {
        if (prompt.type == ERR) {
            return EXIT_FAILURE;
        } else if (prompt.type == BYE) {
            state = NULL;
        } else if (prompt.type != LOCAL) {
            copy(client.msg_content, "Invalid message received", sizeof(client.msg_content));
            SEND(ERR)
            if (prompt.type != ERROR) {
                fprintf(stdout, "ERROR: Received an unexpected message\n");
            }
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}

/**
 * @brief Authentication state, sends AUTH and waits for REPLY/NREPLY.
 * 
 * On REPLY, proceeds to `state_open`. On NREPLY, stays in this state.
 * Terminates on ERR or BYE. Any unexpected message triggers ERR response.
 */
int state_auth(void) {
    prompt_t prompt = await();
    if (prompt.input_type == USER) {
        if (prompt.type == AUTH) {
            comm->processing = true;
            SEND(AUTH)
        } else if (prompt.type == ERROR) {
            return EXIT_FAILURE;
        } else if (prompt.type != LOCAL) {
            fprintf(stdout, "ERROR: Authentication required\n");
        }
    } else {
        if (prompt.type == REPLY) {
            comm->processing = false;
            state = state_open;
        } else if (prompt.type == NREPLY) {
            comm->processing = false;
        } else if (prompt.type == ERR) {
            return EXIT_FAILURE;
        } else if (prompt.type == BYE) {
            state = NULL;
        } else if (prompt.type != LOCAL) {
            copy(client.msg_content, "Server is not responding", sizeof(client.msg_content));
            SEND(ERR)
            if (prompt.type != ERROR) {
                fprintf(stdout, "ERROR: Received an unexpected message\n");
            }
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}

/**
 * @brief Open communication state.
 * 
 * Allows the client to send messages or join a channel.
 * Transitions to `state_join` on JOIN command. Terminates on ERR/BYE.
 */
int state_open(void) {
    prompt_t prompt = await();
    if (prompt.input_type == USER) {
        if (prompt.type == MSG) {
            SEND(MSG)
        } else if (prompt.type == JOIN) {
            comm->processing = true;
            SEND(JOIN)
            state = state_join;
        } else if (prompt.type == ERROR) {
            return EXIT_FAILURE;
        } else if (prompt.type != LOCAL) {
            fprintf(stdout, "ERROR: Invalid command\n");
        }
    } else {
        if (prompt.type == ERR) {
            return EXIT_FAILURE;
        } else if (prompt.type == BYE) {
            state = NULL;
        } else if (prompt.type != MSG && prompt.type != LOCAL) {
            copy(client.msg_content, "Invalid message received", sizeof(client.msg_content));
            SEND(ERR)
            if (prompt.type != ERROR) {
                fprintf(stdout, "ERROR: Received an unexpected message\n");
            }
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}

/**
 * @brief State while waiting for a response after JOIN command.
 * 
 * Transitions back to `state_open` after REPLY or NREPLY.
 * Terminates on ERR or BYE. Unexpected messages trigger ERR.
 */
int state_join(void) {
    prompt_t prompt = await();
    if (prompt.type == REPLY || prompt.type == NREPLY) {
        comm->processing = false;
        state = state_open;
    } else if (prompt.type == ERR) {
        return EXIT_FAILURE;
    } else if (prompt.type == BYE) {
        state = NULL;
    } else if (prompt.type != MSG && prompt.type != LOCAL) {
        copy(client.msg_content, "Server is not responding", sizeof(client.msg_content));
        SEND(ERR)
        if (prompt.type != ERROR) {
            fprintf(stdout, "ERROR: Received an unexpected message\n");
        }
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

/**
 * @brief Graceful termination state after user interrupt.
 * 
 * Sends BYE to server and ends the FSM.
 */
int state_interupt(void) {
    SEND(BYE)
    state = NULL;
    return EXIT_SUCCESS;
}

/**
 * @brief Runs the entire FSM starting in `state_start`.
 * 
 * Initializes the client's display name, checks for interrupt,
 * and runs state transitions until completion or error.
 */
int fsm(void) {
    int res = 0;
    state = state_start;
    copy(client.display_name, "unknown", sizeof(client.display_name));

    while (state && res == 0) {
        if (!comm->processing && interrupt == 1) {
            state = state_interupt;
        }
        res = state();
    }
    clean_up_comm();
    return res;
}
