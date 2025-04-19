/**
 * @file common.c
 * @brief Utility functions for string validation and safe copying.
 * 
 * @author Samuel Stefanik (xstefas00)
 * @date 2025-04-19
 */
#include "../lib/common.h"

/**
 * @brief Validates that a string contains only alphanumeric characters,
 *        hyphens, or underscores, and is within the given length.
 * 
 * @param str The string to validate.
 * @param max_len The maximum allowed length for the string.
 * @return true if valid, false otherwise.
 * 
 * @note This functions was created using ChatGPT
 */
bool is_valid_alphanum_underscore(char *str, int max_len) {
    if ((int)strlen(str) > max_len || strlen(str) < 1) {
        return false;
    }
    for (int i = 0; str[i]; i++) {
        if (!isalnum(str[i]) && str[i] != '-' && str[i] != '_') {
            return false;
        }
    }
    return true;
}

/**
 * @brief Validates that a string contains only printable ASCII characters
 *        (excluding space), and is within the given length.
 * 
 * @param str The string to validate.
 * @param max_len The maximum allowed length for the string.
 * @return true if valid, false otherwise.
 * 
 * @note This functions was created using ChatGPT
 */
bool is_valid_printable(char *str, int max_len) {
    if ((int)strlen(str) > max_len || strlen(str) < 1) {
        return false;
    }
    for (int i = 0; str[i]; i++) {
        if (str[i] < 0x21 || str[i] > 0x7E) {
            return false;
        }
    }
    return true;
}

/**
 * @brief Validates that a string contains valid message content,
 *        including newlines and printable ASCII characters (including space),
 *        and is within the given length.
 * 
 * @param str The string to validate.
 * @param max_len The maximum allowed length for the string.
 * @return true if valid, false otherwise.
 * 
 * @note This functions was created using ChatGPT
 */
bool is_valid_msg_content(char *str, int max_len) {
    if ((int)strlen(str) > max_len || strlen(str) < 1) {
        return false;
    }
    for (int i = 0; str[i]; i++) {
        if (!(str[i] == 0x0A || (str[i] >= 0x20 && str[i] <= 0x7E))) {
            return false;
        }
    }
    return true;
}

/**
 * @brief Safely copies a string with null-termination.
 * 
 * @param dest Destination buffer.
 * @param src Source string.
 * @param size Size of the destination buffer.
 * 
 * @note This functions was created using ChatGPT
 */
void copy(char *dest, char *src, int size) {
    strncpy(dest, src, size - 1);
    dest[size - 1] = '\0';
}
