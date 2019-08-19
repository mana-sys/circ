#include <unistd.h>

#include "connection.h"
#include "handlers.h"
#include "parser.h"
#include "read_message.h"

int handle_read(conn_s *conn)
{
    char *crlf;
    size_t messageLen, remainingLen;
    ssize_t numRead;
    irc_message_s message;

    /*
     * Perform a non-blocking read to try to fill up the rest of the
     * store buffer. The amount of bytes to try to read is IRC_MSG_SIZE
     * - totalRead.
     */
    numRead = read(conn->client.fd, conn->store, conn->totalRead);
    if (numRead == -1)
        return numRead;

    conn->totalRead += numRead;

    /*
     * Handle all messages within the store buffer.
     */
    while ((crlf = strncrlf(conn->store, conn->totalRead))) {

        messageLen = crlf + 2 - conn->store;            /* The length in bytes of the found message. */
        remainingLen = conn->totalRead - messageLen;    /* The length in bytes of the remaining bytes in the store buffer. */

        /*
         * If we are to discard the next message, discard it by overwriting
         * it with the remaining bytes in the buffer.
         */
        if (conn->discardNext) {

            conn->discardNext = false;
            memcpy(conn->store, crlf + 2, remainingLen);
            memset(conn->store + remainingLen, 0, IRC_MSG_SIZE - remainingLen);

            continue;
        }

        /*
         * Else copy the message to the message buffer, parse the message, and
         * handle it.
         */
        memcpy(conn->message, conn->store, messageLen);
        memcpy(conn->store, crlf + 2, remainingLen);
        memset(conn->store + remainingLen, 0, IRC_MSG_SIZE - remainingLen);

        /*
         * Parse the message.
         */
        parse_message(conn->message, &message);

        /*
         * Handle the message.
         */
        handle_message1(&conn->client, NULL, &message, conn->response);

    }

    return 0;
}