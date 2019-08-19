#ifndef READ_MESSAGE_H
#define READ_MESSAGE_H

#include <stdbool.h>
#include <stdlib.h>

/**
 * Returns a pointer to the first occurrence of the CRLF sequence within the
 * specified string, up to the specified limit. If the CRLF sequence is not
 * found, then returns a null pointer.
 * @param str C string to be scanned.
 * @param limit C Number of characters to be scanned within str.
 * @return A pointer to the first sequence in str of the CRLF sequence, or
 * a null pointer if the sequence is not present.
 */
char *strncrlf(char *str, size_t limit);


ssize_t read_message(int fd, char *msg, char *buf, ssize_t *storeTotalRead, bool *storeDiscardNext);

#endif
