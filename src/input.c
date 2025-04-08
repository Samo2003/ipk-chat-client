#include "../lib/input.h"

bool is_valid_alphanum_underscore(char *str, size_t max_len) {
    if (strlen(str) > max_len) {
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
    if (strlen(str) > max_len) {
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
    if (strlen(str) > max_len) {
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

static void  print_client_help(void) {
    fprintf(stdout, "TOODO\n");
}

static msg_type_t parse_auth(char **args) {
    if (!args[0] || !args[1] || !args[2] || args[3] ||
        !is_valid_alphanum_underscore(args[0], 20) ||
        !is_valid_alphanum_underscore(args[1], 128) ||
        !is_valid_printable(args[2], 20)) {
        fprintf(stdout, "ERROR: Invalid /auth\n");
        return LOCAL;
    }

    copy(client.username, args[0], sizeof(client.username));
    copy(client.secret, args[1], sizeof(client.secret));
    copy(client.display_name, args[2], sizeof(client.display_name));
    return AUTH;
}

static msg_type_t parse_join(char **args) {
    if (!args[0] || args[1] || !is_valid_alphanum_underscore(args[0], 20)) {
        fprintf(stdout, "ERROR: Invalid /join\n");
        return LOCAL;
    }

    copy(client.channel_ID, args[0], sizeof(client.channel_ID));
    return JOIN;
}

static msg_type_t parse_rename(char **args) {
    if (!args[0] || args[1] || !is_valid_printable(args[0], 20)) {
        fprintf(stdout, "ERROR: Invalid /rename\n");
        return LOCAL;
    }

    copy(client.display_name, args[0], sizeof(client.display_name));
    return LOCAL;
}

static msg_type_t parse_help(char **args) {
    if (args[0]) {
        fprintf(stdout, "ERROR: invalid /help\n");
        return LOCAL;
    }
    print_client_help();
    return LOCAL;
}

static const command_t command[] = {
    {"/auth", parse_auth},
    {"/join", parse_join},
    {"/rename", parse_rename},
    {"/help", parse_help}
};

msg_type_t parse_user_input(void) {
    char *line = NULL;
    size_t len = 0;

    if (getline(&line, &len, stdin) < 0) {
        if (feof(stdin)) {
            interrupt = 1;
            return LOCAL;
        }
        perror("readline error");
        return ERROR;
    }
    line[strcspn(line, "\n")] = '\0';
    char *orig_line = strdup(line);

    char *tokens[5] = {0};
    int count = 0;
    char *token = strtok(line, " ");
    while (token && count < 5) {
        tokens[count++] = token;
        token = strtok(NULL, " ");
    }
    if (!tokens[0]) {
        fprintf(stdout, "ERROR: Invalid input\n");
        return LOCAL;
    }

    for (int i = 0; i < COMMAND_COUNT; i++) {
        if (strcmp(tokens[0], command[i].cmd) == 0) {
            msg_type_t ret = command[i].parse(&tokens[1]);
            free(line);
            free(orig_line);
            return ret;
        }
    }
    if (!is_valid_msg_content(line, 60000)) {
        fprintf(stdout, "ERROR: Invalid message\n");
        free(line);
        free(orig_line);
        return LOCAL;
    }
    copy(client.msg_content, orig_line, sizeof(client.msg_content));
    free(line);
    free(orig_line);
    return MSG;
}
