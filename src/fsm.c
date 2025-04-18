#include "../lib/fsm.h"

fsm_fn state = NULL;

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

int state_interupt(void) {
    SEND(BYE)
    state = NULL;
    return EXIT_SUCCESS;
}

int fsm(void) {
    int res = 0;
    state = state_start;
    copy(client.display_name, "unknown", sizeof(client.display_name));

    while (state && res == 0) {
        if (!comm->processing && !comm->incomplete && interrupt == 1) {
            state = state_interupt;
        }
        res = state();
    }
    clean_up_comm();
    return res;
}
