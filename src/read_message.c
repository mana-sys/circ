#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "read_message.h"

char *strncrlf(char *str, size_t limit)
{
    for (size_t i = 1; i < limit; i++) {
        if (str[i] == '\n' && str[i - 1] == '\r') {
            return str + i - 1;
        }
    }

    return NULL;
}

static int read_to_CRLF(int fd, char *buf, ssize_t *totalRead, bool *gotCRLF, ssize_t *msgLen)
{
    ssize_t numRead;
    char * crlfIdx;
    while ((crlfIdx = strncrlf(buf, *totalRead)) == NULL) {
        numRead = read(fd, buf + *totalRead, 512 - *totalRead);
        if (numRead == 0 || numRead == -1) {
            return numRead;
        }
        *totalRead += numRead;
        if (*totalRead >= 512) {
            return 1;
        }
    }
    *gotCRLF = true;
    *msgLen = crlfIdx + 2 - buf;
    return 1;
}

ssize_t read_message(int fd, char *msg, char *buf, ssize_t *storeTotalRead, bool *storeDiscardNext)
{
    ssize_t msgLen;
    bool gotCRLF = false;
    int res;

    for (;;) {

        /*
         * First, see if we can read a new message with what we already have inside buf.
         */
        res = read_to_CRLF(fd, buf, storeTotalRead, &gotCRLF, &msgLen);
        if (res == 0 || res == -1) return res;

        /*
         * If we find "\r\n", then we have a message to read. However, it is possible that this message
         * is the ending fragment of a message that exceeded 512 bytes in length. We check if we should
         * discard this ending fragment. If so, discard the fragment and mark the discard flag as false.
         * If we don't, then copy the message to the output message buffer, and shift the bookkeeping
         * buffer to remove that we found.
         */
        if (gotCRLF) {
            if (*storeDiscardNext) {
                *storeDiscardNext = false;
                memmove(buf, buf + msgLen, *storeTotalRead - msgLen);
                memset(buf + (*storeTotalRead - msgLen), 0, 512 - (*storeTotalRead - msgLen));
                continue;
            } else {
                memcpy(msg, buf, msgLen);
                memmove(buf, buf + msgLen, *storeTotalRead - msgLen);
                memset(buf + (*storeTotalRead - msgLen), 0, 512 - (*storeTotalRead - msgLen));
                *storeTotalRead -= msgLen;
                return msgLen;
            }

        /*
         *
         */
        } else {
            *storeTotalRead = 0;

            if (*storeDiscardNext) {
                memset(buf, 0, 512);

            } else {
                *storeDiscardNext = true;
                memcpy(msg, buf, 512);
                msg[512 - 2] = '\r';
                msg[512 - 1] = '\n';
                return 512;
            }

        }
    }
}

char * conn_read_message (conn_s *conn)
{
    char *crlf;
    size_t messageLen, remainingLen;
    ssize_t numRead;

    /*
     * Loop until we retrieve a message, or until it is determined there is no
     * message to retrieve with the current contents of the buffer.
     */
    while (1) {
        crlf = strncrlf(conn->store, conn->totalRead);

        /*
         * Check if the CRLF sequence has been found. If so, we have a message to retrieve.
         * If we are to discard the next message, discard it and continue the loop.
         * Otherwise store the message in the output buffer and return a pointer to it.
         * In any case, the contents of the buffer will be shifted to remove the old message.
         */
        if (crlf) {

            messageLen = crlf + 2 - conn->store;
            remainingLen = conn->totalRead - messageLen;

            /*
             * Copy the message into the buffer if we are not to discard it.
             */
            if (!conn->discardNext) {
                memcpy(conn->message, conn->store, messageLen);
            }

            /*
             * Shift the bytes that were not part of the message.
             */
            memmove(conn->store, conn->store + messageLen, remainingLen);
            memset(conn->store + remainingLen, 0, messageLen);
            conn->totalRead -= messageLen;

            /*
             * If we discarded the message, then set the discardNext flag
             * to false. Else return the message.
             */
            if (conn->discardNext) {
                conn->discardNext = false;
                continue;
            }

            return conn->message;
        }

        /*
         * Also check if we have 511 or 512 bytes in the buffer, and no CRLF has been found.
         * In this case we can go ahead with truncating the message, and moving the
         * extra bytes to the front of the buffer to be discarded later.
         */
        else if (conn->totalRead > IRC_MSG_SIZE - 2) {

            /*
             * If we are not discarding the next message, then we can copy
             * it to the connection's output buffer.
             */
            if (!conn->discardNext) {

                /*
                 * Copy the message to the buffer, truncating it as follows.
                 */
                memcpy(conn->message, conn->store, IRC_MSG_SIZE - 2);
                conn->message[IRC_MSG_SIZE - 2] = '\r';
                conn->message[IRC_MSG_SIZE - 1] = '\n';
            }

            /*
             * Move the extra bytes to the front of the buffer to be discarded
             * later, and zero out the rest of the buffer.
             */
            if (conn->totalRead == IRC_MSG_SIZE - 1) {                  /* 511 bytes. */
                conn->store[0] = conn->store[IRC_MSG_SIZE - 2];
                memset(&conn->store[1], 0, IRC_MSG_SIZE - 1);
                conn->totalRead = 1;

            } else {                                                    /* 512 bytes. */
                conn->store[0] = conn->store[IRC_MSG_SIZE - 2];
                conn->store[1] = conn->store[IRC_MSG_SIZE - 1];
                memset(&conn->store[2], 0, IRC_MSG_SIZE - 2);
                conn->totalRead = 2;
            }

            /*
             * Unset discardNext flag.
             */
            if (conn->discardNext) {
                conn->discardNext = false;
                continue;
            }

            return conn->message;

        } else {
            return NULL;
        }
    }
}