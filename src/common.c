#include "../lib/common.h"


bool is_valid_alphanum_underscore(char *str, size_t max_len) {
    if (strlen(str) > max_len || strlen(str) < 1) {
        return false;
    }
    for (int i = 0; str[i]; i++) {
        if (!isalnum(str[i]) && str[i] != '-' && str[i] != '_') {
            return false;
        }
    }
    return true;
}

bool is_valid_printable(char *str, size_t max_len) {
    if (strlen(str) > max_len || strlen(str) < 1) {
        return false;
    }
    for (int i = 0; str[i]; i++) {
        if (str[i] < 0x21 || str[i] > 0x7E) {
            return false;
        }
    }
    return true;
}

bool is_valid_msg_content(char *str, size_t max_len) {
    if (strlen(str) > max_len || strlen(str) < 1) {
        return false;
    }
    for (int i = 0; str[i]; i++) {
        if (!(str[i] == 0x0A || (str[i] >= 0x20 && str[i] <= 0x7E))) {
            return false;
        }
    }
    return true;
}

void copy(char *dest, char *src, size_t size) {
    strncpy(dest, src, size - 1);
    dest[size - 1] = '\0';
}
