#ifndef CIRC_MSGTOK_H
#define CIRC_MSGTOK_H

#include <string.h>

#define CRLF "\r\n"

char *msgtok(char *str, size_t *toklen);
char *msgtok_r(char *str, size_t *toklen, char **saveptr);

#endif //CIRC_MSGTOK_H
