#include "../lib/tcp_msg.h"

static bool match(char **buffer_ptr, char *pattern, int offset) {
    int len = strlen(pattern);
    if (strncasecmp(*buffer_ptr + offset, pattern, len) == 0 && strlen(*buffer_ptr + offset + len) > 0) {
        *buffer_ptr += offset + len;
        return true;
    }
    return false;
}

static bool read_till_delimiter(char *buffer, char *dest, int size) {
    char *msg_end = strstr(buffer, "\r\n");
    if (msg_end == NULL) {
        return false;
    }
    int len = msg_end - buffer;
    if (len >= size) {
        len = size;
    }
    memcpy(dest, buffer, len);
    dest[len] = '\0';
    return true;
}

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

    msg = malloc(len + ZERO_BYTE);
    if (!msg) {
        return NULL;
    }
    switch (type) {
        case ERR:
        case MSG:
            snprintf(msg, len + ZERO_BYTE, temp, client.display_name, client.msg_content);
            break;
        case AUTH:
            snprintf(msg, len + ZERO_BYTE, temp, client.username, client.display_name, client.secret);
            break;
        case JOIN:
            snprintf(msg, len + ZERO_BYTE, temp, client.channel_ID, client.display_name);
            break;
        case BYE:
            snprintf(msg, len + ZERO_BYTE, temp, client.display_name);
            break;
        default:
            fprintf(stderr, "ERROR: unknown message type\n");
            free(msg);
            return NULL;
    }

    *msg_len = len;
    return msg;
}

msg_type_t parse_reply(char *buffer) {
    char msg_content[BUFFER_SIZE + ZERO_BYTE];
    if (match(&buffer, "REPLY OK IS ", 0) && read_till_delimiter(buffer, msg_content, BUFFER_SIZE) && is_valid_msg_content(msg_content, MAX_MSG_SIZE)) {
        fprintf(stdout, "Action Success: %s\n", msg_content);
        return REPLY;
    }
    fprintf(stdout, "ERROR: Received invalid REPLY\n");
    return ERROR;
}

msg_type_t parse_nreply(char *buffer) {
    char msg_content[BUFFER_SIZE + ZERO_BYTE];
    if (match(&buffer, "REPLY NOK IS ", 0) && read_till_delimiter(buffer, msg_content, BUFFER_SIZE) && is_valid_msg_content(msg_content, MAX_MSG_SIZE)) {
        fprintf(stdout, "Action Failure: %s\n", msg_content);
        return NREPLY;
    }
    fprintf(stdout, "ERROR: Received invalid REPLY\n");
    return ERROR;
}

msg_type_t parse_unexpected(char *buffer) {
    fprintf(stdout, "ERROR: Received an invalid message: %s", buffer);
    return ERROR;
}

msg_type_t parse_msg(char *buffer) {
    char display_name[MAX_NAME_SIZE + ZERO_BYTE];
    char msg_content[BUFFER_SIZE + ZERO_BYTE];
    if (match(&buffer, "MSG FROM ", 0) && sscanf(buffer, "%20s", display_name) == 1 && 
        is_valid_printable(display_name, MAX_NAME_SIZE) && match(&buffer, " IS ", strlen(display_name)) &&
        read_till_delimiter(buffer, msg_content, BUFFER_SIZE) && is_valid_msg_content(msg_content, MAX_MSG_SIZE)) {
        fprintf(stdout, "%s: %s\n", display_name, msg_content);
        return MSG;
    }
    fprintf(stdout, "ERROR: Received invalid MSG\n");
    return ERROR;
}

msg_type_t parse_err(char *buffer) {
    char display_name[MAX_NAME_SIZE + ZERO_BYTE];
    char msg_content[BUFFER_SIZE + ZERO_BYTE];
    if (match(&buffer, "ERR FROM ", 0) && sscanf(buffer, "%20s", display_name) == 1 && 
        is_valid_printable(display_name, MAX_NAME_SIZE) && match(&buffer, " IS ", strlen(display_name)) &&
        read_till_delimiter(buffer, msg_content, BUFFER_SIZE) && is_valid_msg_content(msg_content, MAX_MSG_SIZE)) {
        fprintf(stdout, "ERROR FROM %s: %s\n", display_name, msg_content);
        return ERR;
    }
    fprintf(stdout, "ERROR: Received invalid ERR\n");
    return ERROR;
}

msg_type_t parse_bye(char *buffer) {
    char display_name[MAX_NAME_SIZE + ZERO_BYTE + ZERO_BYTE];
    if (match(&buffer, "BYE FROM ", 0) && read_till_delimiter(buffer, display_name, MAX_NAME_SIZE + ZERO_BYTE) && 
        is_valid_printable(display_name, MAX_NAME_SIZE)) {
        return BYE;
    }
    fprintf(stdout, "ERROR: Received invalid BYE\n");
    return ERROR;
}
