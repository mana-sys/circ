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
        res = read_to_CRLF(fd, buf, storeTotalRead, &gotCRLF, &msgLen);
        if (res == 0 || res == -1) return res;
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