#include "../lib/tcp_msg.h"

char *tcp_build_msg(msg_type_t type, int *msg_len) {
    const char *temp = NULL;
    for (int i = 0; i < TEMP_COUNT; i++) {
        if (msg_temp[i].type == type) {
            temp = msg_temp[i].temp;
            break;
        }
    }
    if (temp == NULL) {
        fprintf(stderr, "ERROR: unknown message type\n");
        return NULL;
    }
    char *message = NULL;
    int len = 0;

    switch (type) {
        case ERR:
        case MSG:
            len = snprintf(NULL, 0, temp, client.display_name, client.msg_content);
            break;
        case AUTH:
            len = snprintf(NULL, 0, temp, client.username, client.display_name, client.secret);
            break;
        case JOIN:
            len = snprintf(NULL, 0, temp, client.channel_ID, client.display_name);
            break;
        case BYE:
            len = snprintf(NULL, 0, temp, client.display_name);
            break;
        default:
            fprintf(stderr, "ERROR: unknown message type\n");
            return NULL;
    }

    message = malloc(len + 1);
    if (message == NULL) {
        fprintf(stderr, "ERROR: memory allocation failed\n");
        return NULL;
    }

    switch (type) {
        case ERR:
        case MSG:
            snprintf(message, len + 1, temp, client.display_name, client.msg_content);
            break;
        case AUTH:
            snprintf(message, len + 1, temp, client.username, client.display_name, client.secret);
            break;
        case JOIN:
            snprintf(message, len + 1, temp, client.channel_ID, client.display_name);
            break;
        case BYE:
            snprintf(message, len + 1, temp, client.display_name);
            break;
        default:
            fprintf(stderr, "ERROR: unknown message type\n");
            return NULL;
    }

    *msg_len = len + 1;
    return message;
}

msg_type_t parse_reply(char *buffer) {
    char msg_content[60001];
    if (sscanf(buffer, "REPLY OK IS %60000[^\r]", msg_content) == 1) {
        fprintf(stdout, "Action Success: %s\n", msg_content);
        return REPLY;
    }
    return ERROR;
}

msg_type_t parse_nreply(char *buffer) {
    char message_content[60001];
    if (sscanf(buffer, "REPLY NOK IS %60000[^\r]", message_content) == 1) {
        fprintf(stdout, "Action Failure: %s\n", message_content);
        return NREPLY;
    }
    return ERROR;
}

msg_type_t parse_unexpected(char *buffer) {
    (void)buffer;
    return ERROR;
}

msg_type_t parse_msg(char *buffer) {
    char display_name[21];
    char msg_content[60001];
    if (sscanf(buffer, "MSG FROM %20s IS %60000[^\r]", display_name, msg_content) == 2) {
        fprintf(stdout, "%s: %s\n", display_name, msg_content);
        return MSG;
    }
    return ERROR;
}

msg_type_t parse_err(char *buffer) {
    char display_name[21];
    char msg_content[60001];
    if (sscanf(buffer, "ERR FROM %20s IS %60000[^\r]", display_name, msg_content) == 2) {
        fprintf(stdout, "ERROR FROM %s: %s\n", display_name, msg_content);
        return ERR;
    }
    return ERROR;
}

msg_type_t parse_bye(char *buffer) {
    char display_name[21];
    if (sscanf(buffer, "BYE FROM %20s", display_name) == 1) {
        return BYE;
    }
    return ERROR;
}
