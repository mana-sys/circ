#ifndef READ_MESSAGE_H
#define READ_MESSAGE_H

#include <stdbool.h>
#include <stdlib.h>

#include "connection.h"

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


ssize_t read_message      (int fd, char *msg, char *buf, ssize_t *storeTotalRead, bool *storeDiscardNext);

/**
 * Attempt to perform a message read for the specified connection based
 * on the state of the connection. If the connection's flag to discard
 * the next message is set, then this function will discard at most one
 * existing message, if it finds one, and will reattempt the read.
 *
 * @param conn The connection on which to perform a message read on.
 * @return A pointer to the message that was read, or a null pointer if no
 * message could be read.
 */
// TODO: Handle corner cases:
//   - Message of length 513 (i.e. message[511] = '\r', message[512] = '\n'
char * conn_read_message (conn_s *conn);



#endif
