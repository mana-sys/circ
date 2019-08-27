#ifndef CIRC_MSGTOK_H
#define CIRC_MSGTOK_H

#include <string.h>

#define CRLF "\r\n"

/**
 * Tokenizes an IRC message in a manner similar to strtok(), but
 * considers IRC tokens.
 *
 * @param str The IRC message string to be tokenized.
 * @param toklen A buffer where the token length is returned.
 * @return A pointer to the next token in the message.
 */
char *msgtok(char *str, size_t *toklen);

/**
 * Re-entrant version of msgtok.
 *
 * @param str The IRC message string to be tokenized.
 * @param toklen A buffer where the token length is returned.
 * @param saveptr Saves location of the next parsing location.
 * @return A pointer to the next token in the message.
 */
char *msgtok_r(char *str, size_t *toklen, char **saveptr);

#endif //CIRC_MSGTOK_H
