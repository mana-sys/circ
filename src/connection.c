#include <unistd.h>

#include "connection.h"
#include "handlers.h"
#include "log.h"
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
    numRead = read(conn->client.fd, conn->store, IRC_MSG_SIZE - conn->totalRead);
    circlog(L_TRACE, "Read %ld bytes", numRead);
    if (numRead == -1)
        return numRead;

    conn->totalRead += numRead;

    /*
     * Handle all messages within the store buffer.
     */
    while (conn_read_message(conn)) {
        circlog(L_DEBUG, "Received message: '%s'", conn->message);

        parse_message(conn->message, &message);
        handle_message1(&conn->client, NULL, &message, conn->response);
    }

    /*
     * If we read 0 bytes, then the client closed the connection.
     * We close our side of the connection.
     */
    if (numRead == 0) {
        close(conn->client.fd);
    }

    return 0;
}