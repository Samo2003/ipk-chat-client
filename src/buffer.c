#include "../lib/buffer.h"

msg_buffer_t msg_buffer = {0};

static char *crlf(void) {
    for (int i = 0; i < msg_buffer.msg_len - 1; ++i) {
        if (msg_buffer.msg[i] == '\r' && msg_buffer.msg[i+1] == '\n') {
            return &msg_buffer.msg[i];
        }
    }
    return NULL;
}

bool buffer_append(char *msg, int msg_len) {
    if (msg_buffer.msg_len + msg_len > msg_buffer.size) {
        int new_size = msg_buffer.size + BUFFER_SIZE;
        char *new_msg = realloc(msg_buffer.msg, new_size);
        if (!new_msg) {
            return false;
        }
        msg_buffer.msg = new_msg;
        msg_buffer.size = new_size;
    }
    copy(msg_buffer.msg + msg_buffer.msg_len, msg, msg_len + ZERO_BYTE);
    msg_buffer.msg_len += msg_len;
    return true;
}

void buffer_destroy(void) {
    if (msg_buffer.msg) {
        free(msg_buffer.msg);
        msg_buffer.msg = NULL;
        msg_buffer.msg_len = 0;
        msg_buffer.size = 0;
    }
}

char *buffer_get_msg(void) {
    if (buffer_is_empty()) {
        return NULL;
    }
    char *end = crlf();
    if (!end) {
        return NULL;
    }
    int msg_len = end - msg_buffer.msg + 2;
    char *msg = malloc(msg_len + 1);
    if (!msg) {
        return NULL;
    }
    memcpy(msg, msg_buffer.msg, msg_len);
    msg[msg_len] = '\0';
    memmove(msg_buffer.msg, msg_buffer.msg + msg_len, msg_buffer.msg_len - msg_len);
    msg_buffer.msg_len -= msg_len;
    return msg;
}

bool buffer_is_empty(void) {
    return msg_buffer.msg_len == 0;
}
