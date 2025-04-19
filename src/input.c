/**
 * @file input.c
 * @brief Handles parsing and validation of user input from stdin.
 * 
 * @author Samuel Stefanik (xstefas00)
 * @date 2025-04-19
 */
#include "../lib/input.h"

/**
 * @brief Displays a help message listing available client commands.
 * 
 * @note This functions was created using ChatGPT
 */
static void  print_client_help(void) {
    fprintf(stdout,
        "Client Commands:\n"
        "  /auth {Username} {Secret} {DisplayName}\n"
        "      Authenticate with the server using the provided credentials.\n\n"
        "  /join {ChannelID}\n"
        "      Join the specified chat channel.\n\n"
        "  /rename {DisplayName}\n"
        "      Change your local display name (used when sending messages).\n\n"
        "  /help\n"
        "      Show this help message.\n"
    );
}

/**
 * @brief Parses the /auth command and updates client credentials.
 * 
 * @param args Array of strings containing the command arguments.
 * @return AUTH on success, LOCAL on invalid input.
 */
static msg_type_t parse_auth(char **args) {
    if (!args[0] || !args[1] || !args[2] || args[3] ||
        !is_valid_alphanum_underscore(args[0], MAX_NAME_SIZE) ||
        !is_valid_alphanum_underscore(args[1], MAX_SECRET_SIZE) ||
        !is_valid_printable(args[2], 20)) {
        fprintf(stdout, "ERROR: Invalid /auth\n");
        return LOCAL;
    }

    copy(client.username, args[0], sizeof(client.username));
    copy(client.secret, args[1], sizeof(client.secret));
    copy(client.display_name, args[2], sizeof(client.display_name));
    return AUTH;
}

/**
 * @brief Parses the /join command and updates the client's channel ID.
 * 
 * @param args Array of strings containing the command arguments.
 * @return JOIN on success, LOCAL on invalid input.
 */
static msg_type_t parse_join(char **args) {
    if (!args[0] || args[1] || !is_valid_alphanum_underscore(args[0], MAX_NAME_SIZE)) {
        fprintf(stdout, "ERROR: Invalid /join\n");
        return LOCAL;
    }

    copy(client.channel_ID, args[0], sizeof(client.channel_ID));
    return JOIN;
}

/**
 * @brief Parses the /rename command and updates the client's display name.
 * 
 * @param args Array of strings containing the command arguments.
 * @return LOCAL, whether success or error (affects only local state).
 */
static msg_type_t parse_rename(char **args) {
    if (!args[0] || args[1] || !is_valid_printable(args[0], MAX_NAME_SIZE)) {
        fprintf(stdout, "ERROR: Invalid /rename\n");
        return LOCAL;
    }

    copy(client.display_name, args[0], sizeof(client.display_name));
    return LOCAL;
}

/**
 * @brief Parses the /help command and displays help if valid.
 * 
 * @param args Array of strings containing the command arguments.
 * @return LOCAL always.
 */
static msg_type_t parse_help(char **args) {
    if (args[0]) {
        fprintf(stdout, "ERROR: Invalid /help\n");
        return LOCAL;
    }
    print_client_help();
    return LOCAL;
}

/**
 * @brief Table of supported commands and their corresponding parser functions.
 */
static const command_t command[] = {
    {"/auth", parse_auth},     /**< Command for authenticating the user. */
    {"/join", parse_join},     /**< Command for joining a chat channel. */
    {"/rename", parse_rename}, /**< Command for changing display name. */
    {"/help", parse_help}      /**< Command for displaying help. */
};

/**
 * @brief Parses a full line of user input from stdin.
 * 
 * This function supports command parsing (starting with /) or sending
 * general messages. It performs input validation and handles truncation
 * if the message exceeds MAX_MSG_SIZE.
 * 
 * @return A value of type msg_type_t representing the parsed message type.
 */
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
    line[strcspn(line, "\n")] = '\0';   // Remove newline character
    char *orig_line = strdup(line);     // Preserve full original input

    char *tokens[5] = {0};
    int count = 0;
    char *token = strtok(line, " ");
    while (token && count < 5) {
        tokens[count++] = token;
        token = strtok(NULL, " ");
    }

    if (tokens[0] && tokens[0][0] == '/') {
        for (int i = 0; i < COMMAND_COUNT; i++) {
            if (strcmp(tokens[0], command[i].cmd) == 0) {
                msg_type_t ret = command[i].parse(&tokens[1]);
                free(line);
                free(orig_line);
                return ret;
            }
        }
        free(line);
        free(orig_line);
        fprintf(stdout, "ERROR: Invalid command\n");
        return LOCAL;
    }
    if (!is_valid_msg_content(orig_line, BUFFER_SIZE)) {
        fprintf(stdout, "ERROR: Invalid message\n");
        free(line);
        free(orig_line);
        return LOCAL;
    }
    if (strlen(orig_line) > MAX_MSG_SIZE) {
        orig_line[MAX_MSG_SIZE] = '\0';
        fprintf(stdout, "ERROR: Truncated message sent\n");
    }
    copy(client.msg_content, orig_line, sizeof(client.msg_content));
    free(line);
    free(orig_line);
    return MSG;
}
