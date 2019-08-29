#ifndef PARSER_H
#define PARSER_H

#include "irc.h"

/**
 * Parses an IRC message from the specified buffer into the specified
 * message struct. To avoid excess allocations, the message struct will
 * modify the provided string and hold pointers to the tokens in the buffer,
 * so the buffer should not be modified, lest the message structure's contents
 * also be modified.
 *
 * @param buffer The buffer from which to parse the IRC message.
 * @param message Message struct to hold the parsed message.
 * @return
 */
int parse_message(char *buffer, struct irc_message *message);

#endif
