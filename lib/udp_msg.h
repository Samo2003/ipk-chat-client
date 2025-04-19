/**
 * @file udp_msg.h
 * @brief Contains function definitions to build and parse UDP protocol variant messages.
 *
 * @author Samuel Stefanik (xstefas00)
 * @date   2025-04-19
 */
#ifndef UDP_MSG_H
#define UDP_MSG_H

#include "args.h"
#include "list.h"
#include "queue.h"

#define HEADER_SIZE 3   /**< UDP protocol variant header message size (message type, message ID) */

/**
 * @brief Constructs the header of a UDP message.
 *
 * @param type Type of the message.
 * @param msg_id Message identifier.
 * @param payload_len Length of the message payload.
 * @return Pointer to the constructed message header or NULL on failure.
 */
char *build_header(msg_type_t type, uint16_t msg_id, int payload_len);

/**
 * @brief Builds a UDP message based on the message type.
 *
 * @param type Message type to build.
 * @param msg_len Pointer to store the total message length.
 * @return Pointer to the constructed message or NULL on failure.
 */
char *udp_build_msg(msg_type_t type, int *msg_len);

/**
 * @brief Converts two bytes into a 16-bit unsigned integer.
 *
 * @param buffer Pointer to the buffer containing the two bytes.
 * @return 16-bit unsigned integer value.
 */
uint16_t to_uint16(char *buffer);

/**
 * @brief Parses a CONFIRM message and checks its validity.
 *
 * @param buffer Pointer to the received message.
 * @param msg_len Length of the message.
 * @return LOCAL if confirmation is valid, ERROR otherwise.
 */
msg_type_t udp_parse_confirm(char *buffer, int msg_len);

/**
 * @brief Parses a REPLY or NREPLY message and prints the result.
 *
 * @param buffer Pointer to the received message.
 * @param msg_len Length of the message.
 * @return REPLY or NREPLY if valid, ERROR otherwise.
 */
msg_type_t udp_parse_reply(char *buffer, int msg_len);

/**
 * @brief Handles an unexpected message type.
 *
 * @param buffer Pointer to the received message.
 * @param msg_len Length of the message.
 * @return Always returns ERROR.
 */
msg_type_t udp_parse_unexpected(char *buffer, int msg_len);

/**
 * @brief Parses and prints a MSG message.
 *
 * @param buffer Pointer to the received message.
 * @param msg_len Length of the message.
 * @return MSG if valid, ERROR otherwise.
 */
msg_type_t udp_parse_msg(char *buffer, int msg_len);

/**
 * @brief Parses and prints an ERR message.
 *
 * @param buffer Pointer to the received message.
 * @param msg_len Length of the message.
 * @return ERR if valid, ERROR otherwise.
 */
msg_type_t udp_parse_err(char *buffer, int msg_len);

/**
 * @brief Parses a BYE message and checks its validity.
 *
 * @param buffer Pointer to the received message.
 * @param msg_len Length of the message.
 * @return BYE if valid, ERROR otherwise.
 */
msg_type_t udp_parse_bye(char *buffer, int msg_len);

/**
 * @brief Parses a PING message and validates its format.
 *
 * @param buffer Pointer to the received message.
 * @param msg_len Length of the message.
 * @return PING if valid, ERROR otherwise.
 */
msg_type_t udp_parse_ping(char *buffer, int msg_len);

#endif
