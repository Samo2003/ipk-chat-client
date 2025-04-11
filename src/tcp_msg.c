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
    char *msg = NULL;
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

    if (len > MAX_TCP_PACKET_SIZE) {
        len = MAX_TCP_PACKET_SIZE;
        fprintf(stdout, "ERROR: Input massage content was too long truncated massage sent\n");
    }
    msg = malloc(len + 1);
    if (!msg) {
        return NULL;
    }
    int written = 0;
    switch (type) {
        case ERR:
        case MSG:
            written = snprintf(msg, len + 1, temp, client.display_name, client.msg_content);
            break;
        case AUTH:
            written = snprintf(msg, len + 1, temp, client.username, client.display_name, client.secret);
            break;
        case JOIN:
            written = snprintf(msg, len + 1, temp, client.channel_ID, client.display_name);
            break;
        case BYE:
            written = snprintf(msg, len + 1, temp, client.display_name);
            break;
        default:
            fprintf(stderr, "ERROR: unknown message type\n");
            free(msg);
            return NULL;
    }
    if (written > len - 2) {
        written = len - 2;
    }
    memcpy(msg + written, "\r\n", 2);
    msg[written + 2] = '\0';

    *msg_len = written + 2;
    return msg;
}

msg_type_t parse_reply(char *buffer) {
    char msg_content[MAX_MSG_SIZE + ZERO_BYTE];
    if (sscanf(buffer, "REPLY OK IS %60000[^\r]\r\n", msg_content) == 1 && is_valid_msg_content(msg_content, MAX_MSG_SIZE)) {
        fprintf(stdout, "Action Success: %s\n", msg_content);
        return REPLY;
    }
    fprintf(stdout, "ERROR: Received invalid REPLY\n");
    return ERROR;
}

msg_type_t parse_nreply(char *buffer) {
    char msg_content[MAX_MSG_SIZE + ZERO_BYTE];
    if (sscanf(buffer, "REPLY NOK IS %60000[^\r]\r\n", msg_content) == 1 && is_valid_msg_content(msg_content, MAX_MSG_SIZE)) {
        fprintf(stdout, "Action Failure: %s\n", msg_content);
        return NREPLY;
    }
    fprintf(stdout, "ERROR: Received invalid REPLY\n");
    return ERROR;
}

msg_type_t parse_unexpected(char *buffer) {
    fprintf(stdout, "ERROR: Received invalid message: %s", buffer);
    return ERROR;
}

msg_type_t parse_msg(char *buffer) {
    char display_name[MAX_NAME_SIZE + ZERO_BYTE];
    char msg_content[MAX_MSG_SIZE + ZERO_BYTE];
    if (sscanf(buffer, "MSG FROM %20s IS %60000[^\r]\r\n", display_name, msg_content) == 2 && 
        is_valid_printable(display_name, MAX_NAME_SIZE) && is_valid_msg_content(msg_content, MAX_MSG_SIZE)) {
        fprintf(stdout, "%s: %s\n", display_name, msg_content);
        return MSG;
    }
    fprintf(stdout, "ERROR: Received invalid MSG\n");
    return ERROR;
}

msg_type_t parse_err(char *buffer) {
    char display_name[MAX_NAME_SIZE + ZERO_BYTE];
    char msg_content[MAX_MSG_SIZE + ZERO_BYTE];
    if (sscanf(buffer, "ERR FROM %20s IS %60000[^\r]\r\n", display_name, msg_content) == 2 && 
        is_valid_printable(display_name, MAX_NAME_SIZE) && is_valid_msg_content(msg_content, MAX_MSG_SIZE)) {
        fprintf(stdout, "ERROR FROM %s: %s\n", display_name, msg_content);
        return ERR;
    }
    fprintf(stdout, "ERROR: Received invalid ERR\n");
    return ERROR;
}

msg_type_t parse_bye(char *buffer) {
    char display_name[MAX_NAME_SIZE + ZERO_BYTE];
    if (sscanf(buffer, "BYE FROM %20[^\r]\r\n", display_name) == 1 && is_valid_printable(display_name, MAX_NAME_SIZE)) {
        printf("%s\n", display_name);
        return BYE;
    }
    fprintf(stdout, "ERROR: Received invalid BYE\n");
    return ERROR;
}
