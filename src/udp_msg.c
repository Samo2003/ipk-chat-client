/**
 * @file udp_msg.c
 * @brief Contains functions to build and parse UDP protocol variant messages.
 *
 * @author Samuel Stefanik (xstefas00)
 * @date   2025-04-19
 */
#include "../lib/udp_msg.h"

/**
 * @brief Builds an AUTH message using client data.
 *
 * @param msg_len Pointer to store the length of the created message.
 * @return Pointer to the constructed AUTH message or NULL on failure.
 */
static char *build_auth(int *msg_len) {
    int payload_len = strlen(client.username) + ZERO_BYTE + strlen(client.display_name) + ZERO_BYTE + strlen(client.secret) + ZERO_BYTE;
    char *msg = build_header(AUTH, client.msg_id, payload_len);
    if (!msg) {
        return NULL;
    }
    strcpy(&msg[HEADER_SIZE], client.username);
    strcpy(&msg[HEADER_SIZE + strlen(client.username) + ZERO_BYTE], client.display_name);
    strcpy(&msg[HEADER_SIZE+ strlen(client.username) + ZERO_BYTE + strlen(client.display_name) + ZERO_BYTE], client.secret);
    *msg_len = HEADER_SIZE + payload_len;
    return msg;
}

/**
 * @brief Builds a JOIN message using client data.
 *
 * @param msg_len Pointer to store the length of the created message.
 * @return Pointer to the constructed JOIN message or NULL on failure.
 */
static char *build_join(int *msg_len) {
    int payload_len = strlen(client.channel_ID) + ZERO_BYTE + strlen(client.display_name) + ZERO_BYTE;
    char *msg = build_header(JOIN, client.msg_id, payload_len);
    if (!msg) {
        return NULL;
    }
    strcpy(&msg[HEADER_SIZE], client.channel_ID);
    strcpy(&msg[HEADER_SIZE + strlen(client.channel_ID) + ZERO_BYTE], client.display_name);
    *msg_len = HEADER_SIZE + payload_len;
    return msg;
}

/**
 * @brief Builds a MSG or ERR message using client data.
 *
 * @param msg_len Pointer to store the length of the created message.
 * @param type Type of the message to build (MSG or ERR).
 * @return Pointer to the constructed message or NULL on failure.
 */
static char *build_msg_err(int *msg_len, msg_type_t type) {
    int payload_len = strlen(client.display_name) + ZERO_BYTE + strlen(client.msg_content) + ZERO_BYTE;
    char *msg = build_header(type, client.msg_id, payload_len);
    if (!msg) {
        return NULL;
    }
    strcpy(&msg[HEADER_SIZE], client.display_name);
    strcpy(&msg[HEADER_SIZE + strlen(client.display_name) + ZERO_BYTE], client.msg_content);
    *msg_len = HEADER_SIZE + payload_len;
    return msg;
}

/**
 * @brief Builds a BYE message using client data.
 *
 * @param msg_len Pointer to store the length of the created message.
 * @return Pointer to the constructed BYE message or NULL on failure.
 */
static char *build_bye(int *msg_len) {
    int payload_len = strlen(client.display_name) + ZERO_BYTE;
    char *msg = build_header(BYE, client.msg_id, payload_len);
    if (!msg) {
        return NULL;
    }
    strcpy(&msg[HEADER_SIZE], client.display_name);
    *msg_len = HEADER_SIZE + payload_len;
    return msg;
}

/**
 * @brief Validates the content of a MSG or ERR message.
 *
 * @param buffer Pointer to the received message.
 * @param len Total length of the message.
 * @return true if message is valid, false otherwise.
 */
static bool is_valid_msg_err(char *buffer, int len) {
    if (len < HEADER_SIZE + ZERO_BYTE) {
        return false;
    }
    if (!is_valid_alphanum_underscore(&buffer[HEADER_SIZE], MAX_NAME_SIZE)) {
        return false;
    }
    int display_name_len = strlen(&buffer[HEADER_SIZE]);
    int content_offset = HEADER_SIZE + display_name_len + ZERO_BYTE;
    if (len < content_offset + 1 + ZERO_BYTE) {
        return false;
    }
    if (!is_valid_msg_content(&buffer[content_offset], MAX_MSG_SIZE)) {
        return false;
    }
    if (len != content_offset + (int)strlen(&buffer[content_offset]) + ZERO_BYTE) {
        return false;
    }
    return true;
}

/**
 * @brief Constructs the header of a UDP message.
 *
 * @param type Type of the message.
 * @param msg_id Message identifier.
 * @param payload_len Length of the message payload.
 * @return Pointer to the constructed message header or NULL on failure.
 */
char *build_header(msg_type_t type, uint16_t msg_id, int payload_len) {
    char *msg = malloc(HEADER_SIZE + payload_len);
    if (!msg) {
        return NULL;
    }
    msg[0] = type;
    msg[1] = (msg_id >> 8) & 0xff;
    msg[2] = msg_id & 0xff;
    return msg;
}

/**
 * @brief Builds a UDP message based on the message type.
 *
 * @param type Message type to build.
 * @param msg_len Pointer to store the total message length.
 * @return Pointer to the constructed message or NULL on failure.
 */
char *udp_build_msg(msg_type_t type, int *msg_len) {
    switch (type) {
        case AUTH:
            return build_auth(msg_len);
        case JOIN:
            return build_join(msg_len);
        case MSG:
        case ERR:
            return build_msg_err(msg_len, type);
        case BYE:
            return build_bye(msg_len);
        default:
            *msg_len = 0;
            return NULL;
    }
}

/**
 * @brief Converts two bytes into a 16-bit unsigned integer.
 *
 * @param buffer Pointer to the buffer containing the two bytes.
 * @return 16-bit unsigned integer value.
 */
uint16_t to_uint16(char *buffer) {
    return (uint16_t)(((uint8_t)buffer[0] << 8) | (uint8_t)buffer[1]);
}

/**
 * @brief Parses a CONFIRM message and checks its validity.
 *
 * @param buffer Pointer to the received message.
 * @param msg_len Length of the message.
 * @return LOCAL if confirmation is valid, ERROR otherwise.
 */
msg_type_t udp_parse_confirm(char *buffer, int msg_len) {
    if (msg_len == HEADER_SIZE && to_uint16(&buffer[1]) < client.msg_id) {
        return LOCAL;
    }
    fprintf(stdout, "ERROR: Received an unexpected CONFIRM\n");
    return ERROR;
}

/**
 * @brief Parses a REPLY or NREPLY message and prints the result.
 *
 * @param buffer Pointer to the received message.
 * @param msg_len Length of the message.
 * @return REPLY or NREPLY if valid, ERROR otherwise.
 */
msg_type_t udp_parse_reply(char *buffer, int msg_len) {
    if (msg_len >= 8 && is_valid_msg_content(&buffer[6], MAX_MSG_SIZE) && 
        msg_len == 6 + (int)strlen(&buffer[6]) + ZERO_BYTE && 
        to_uint16(&buffer[4]) == client.msg_id - 1) {
        if (buffer[HEADER_SIZE] == true) {
            printf("Action Success: %s\n", &buffer[6]);
            return REPLY;
        } else {
            printf("Action Failure: %s\n", &buffer[6]);
            return NREPLY;
        }
    }
    fprintf(stdout, "ERROR: Received invalid REPLY\n");
    return ERROR;
}

/**
 * @brief Handles an unexpected message type.
 *
 * @param buffer Pointer to the received message.
 * @param msg_len Length of the message.
 * @return Always returns ERROR.
 */
msg_type_t udp_parse_unexpected(char *buffer, int msg_len) {
    (void)buffer;
    (void)msg_len;
    fprintf(stdout, "ERROR: Received unexpected message\n");
    return ERROR;
}

/**
 * @brief Parses and prints a MSG message.
 *
 * @param buffer Pointer to the received message.
 * @param msg_len Length of the message.
 * @return MSG if valid, ERROR otherwise.
 */
msg_type_t udp_parse_msg(char *buffer, int msg_len) {
    if (is_valid_msg_err(buffer, msg_len)) {
        printf("%s: %s\n", &buffer[HEADER_SIZE], &buffer[HEADER_SIZE + ZERO_BYTE + strlen(&buffer[HEADER_SIZE])]);
        return MSG;
    }
    fprintf(stdout, "ERROR: Received invalid MSG\n");
    return ERROR;
}

/**
 * @brief Parses and prints an ERR message.
 *
 * @param buffer Pointer to the received message.
 * @param msg_len Length of the message.
 * @return ERR if valid, ERROR otherwise.
 */
msg_type_t udp_parse_err(char *buffer, int msg_len) {
    if (is_valid_msg_err(buffer, msg_len)) {
        printf("ERROR FROM %s: %s\n", &buffer[HEADER_SIZE], &buffer[HEADER_SIZE + ZERO_BYTE + strlen(&buffer[HEADER_SIZE])]);
        return ERR;
    }
    fprintf(stdout, "ERROR: Received invalid ERR\n");
    return ERROR;
}

/**
 * @brief Parses a BYE message and checks its validity.
 *
 * @param buffer Pointer to the received message.
 * @param msg_len Length of the message.
 * @return BYE if valid, ERROR otherwise.
 */
msg_type_t udp_parse_bye(char *buffer, int msg_len) {
    if (msg_len >= HEADER_SIZE + 1 + ZERO_BYTE && 
        is_valid_alphanum_underscore(&buffer[HEADER_SIZE], 20) &&
        msg_len == HEADER_SIZE + (int)strlen(&buffer[HEADER_SIZE]) + ZERO_BYTE) {
        return BYE;
    }
    fprintf(stdout, "ERROR: Received invalid BYE\n");
    return ERROR;
}

/**
 * @brief Parses a PING message and validates its format.
 *
 * @param buffer Pointer to the received message.
 * @param msg_len Length of the message.
 * @return PING if valid, ERROR otherwise.
 */
msg_type_t udp_parse_ping(char *buffer, int msg_len) {
    (void)buffer;
    if (msg_len == HEADER_SIZE){
        return PING;
    }
    fprintf(stdout, "ERROR: Received invalid PING\n");
    return ERROR;
}
