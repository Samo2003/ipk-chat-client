/**
 * @file list.c
 * @brief Implementation of a dynamic list used to track message IDs.
 * 
 * @author Samuel Stefanik (xstefas00)
 * @date 2025-04-19
 */
#include "../lib/list.h"

// Global message ID list used to track received messages
list_t msg_id_list = {0};

/**
 * @brief Checks if the given message ID is already in the list.
 * 
 * @param number 16-bit message ID to search for.
 * @return true if the ID exists in the list, false otherwise.
 * 
 * @note This functions was created using ChatGPT
 */
bool list_contains(uint16_t number) {
    for (int i = 0; i < msg_id_list.count; i++) {
        if (msg_id_list.content[i] == number) {
            return true;
        }
    }
    return false;
}

/**
 * @brief Appends a new message ID to the list.
 * 
 * If the list is full, it is dynamically resized by LIST_LEN.
 * 
 * @param number 16-bit message ID to add.
 * @return EXIT_SUCCESS on success, EXIT_FAILURE if reallocation fails.
 * 
 * @note This functions was created using ChatGPT
 */
int list_append(uint16_t number) {
    if (msg_id_list.capacity == msg_id_list.count) {
        msg_id_list.content = realloc(msg_id_list.content, (msg_id_list.capacity + LIST_LEN) * sizeof(uint16_t));
        PTR_CHECK(msg_id_list.content)
        msg_id_list.capacity += LIST_LEN;
    }
    msg_id_list.content[msg_id_list.count++] = number;
    return EXIT_SUCCESS;
}

/**
 * @brief Frees memory used by the message ID list.
 * 
 * Call this function during cleanup to avoid memory leaks.
 * 
 * @note This functions was created using ChatGPT
 */
void list_destroy(void) {
    if (msg_id_list.content) {
        free(msg_id_list.content);
    }
}
