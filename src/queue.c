#include "../lib/queue.h"

msg_queue_t msg_queue = {0};

bool queue_is_empty(void) {
    return msg_queue.size == 0;
}

bool queue_is_full(void) {
    return msg_queue.size == MAX_QUEUE_SIZE;
}

bool queue_add(char *msg, int msg_len) {
    if (queue_is_full()) {
        return false;
    }
    char *copy = malloc(msg_len);
    if (!copy) {
        return false;
    }
    memcpy(copy, msg, msg_len);
    msg_queue.msgs[msg_queue.end].msg = copy;
    msg_queue.msgs[msg_queue.end].msg_len = msg_len;

    msg_queue.end = (msg_queue.end + 1) % MAX_QUEUE_SIZE;
    msg_queue.size++;
    return true;
}

bool queue_front(char **msg, int *msg_len) {
    if (queue_is_empty()) {
        return false;
    }
    *msg = msg_queue.msgs[msg_queue.front].msg;
    *msg_len = msg_queue.msgs[msg_queue.front].msg_len;
    msg_queue.front = (msg_queue.front + 1) % MAX_QUEUE_SIZE;
    msg_queue.size--;
    return true;
}

void queue_destroy(void) {
    while (!queue_is_empty()) {
        char *msg;
        int msg_len;
        queue_front(&msg, &msg_len);
        free(msg);
    }
}
