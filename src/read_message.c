#include <stdio.h>
#include <string.h>

#include "read_message.h"

ssize_t read_message(int fd, char *msg, char *buf, ssize_t *storeTotalRead, read_func_type read_func)
{
    ssize_t numRead, numExtraBytes, msgLen;
    char *msg_end;

    for (;;) {

        numRead = read_func(fd, buf + *storeTotalRead, 512 - *storeTotalRead);
        if (numRead == -1 || numRead == 0)
            return numRead;

        *storeTotalRead += numRead;

        if ((msg_end = strstr(buf, "\r\n")) == NULL) {
            if (*storeTotalRead == 512) {
                fprintf(stderr, "Invalid message in full buffer. Discarding buffer contents.\n");
                memset(buf, 0, 512);
                *storeTotalRead = 0;
            } else {
                fprintf(stderr, "Waiting for more input... (totalRead = %ld)\n", *storeTotalRead);
            }
            continue;
        }

        msgLen = msg_end + 2 - buf;
        numExtraBytes = *storeTotalRead - msgLen;

        memcpy(msg, buf, msgLen);
        memmove(buf, msg_end + 2, numExtraBytes);
        memset(buf + numExtraBytes, 0, 512 - numExtraBytes);

        return msgLen;

    }
}