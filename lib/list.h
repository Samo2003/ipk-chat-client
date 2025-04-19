/**
 * @file list.h
 * @brief Implementation of a dynamic list used to track message IDs.
 *
 * This module provides basic functionality for a dynamically resizing list,
 * specifically used for storing 16-bit message identifiers to detect duplicates.
 * 
 * @author Samuel Stefanik (xstefas00)
 * @date 2025-04-19
 */
#ifndef LIST_H
#define LIST_H

#include "common.h"

#define LIST_LEN 20     /**< Number of elements to allocate during list expansion. */

/**
 * @brief Structure representing a dynamic list of 16-bit message IDs.
 */
typedef struct list {
    int count;           /**< Current number of elements in the list. */
    int capacity;        /**< Current allocated capacity of the list. */
    uint16_t *content;   /**< Pointer to the array holding message IDs. */
} list_t;

// Global message ID list used to track received messages
extern list_t msg_id_list;

/**
 * @brief Checks if the given message ID is already in the list.
 * 
 * @param number 16-bit message ID to search for.
 * @return true if the ID exists in the list, false otherwise.
 * 
 * @note This functions was created using ChatGPT
 */
bool list_contains(uint16_t number);

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
int list_append(uint16_t number);

/**
 * @brief Frees memory used by the message ID list.
 * 
 * Call this function during cleanup to avoid memory leaks.
 * 
 * @note This functions was created using ChatGPT
 */
void list_destroy(void);

#endif
