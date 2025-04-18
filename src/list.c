#include "../lib/list.h"

list_t msg_id_list = {0};

bool list_contains(uint16_t number) {
    for (int i = 0; i < msg_id_list.count; i++) {
        if (msg_id_list.content[i] == number) {
            return true;
        }
    }
    return false;
}

int list_append(uint16_t number) {
    if (msg_id_list.capacity == msg_id_list.count) {
        msg_id_list.content = realloc(msg_id_list.content, (msg_id_list.capacity + LIST_LEN) * sizeof(uint16_t));
        PTR_CHECK(msg_id_list.content)
        msg_id_list.capacity += LIST_LEN;
    }
    msg_id_list.content[msg_id_list.count++] = number;
    return EXIT_SUCCESS;
}

void list_destroy(void) {
    if (msg_id_list.content) {
        free(msg_id_list.content);
    }
}
