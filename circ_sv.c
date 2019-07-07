#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define BACKLOG 5
#define CRLF "\r\n"
#define MSG_SIZE 20
#define NICK "NICK"
#define USER "USER"

#define SV_SOCK_PATH "/tmp/circ.sock"

void exitErr(const char * msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

// strategy
// - try to read up to 512 bytes from buffer
// - check message for \r\n sequence. If it exists, we have read an entire message and can parse.
// - if not exists, need to try to read (512 - numRead) bytes from the buffer. If the substring \r\n is not
// - found here, then that's an error. Otherwise, we parse the message, up to the \r\n. After parsing the message,
// - take the rest of the bytes and copy them to the front of the buffer

int main(int argc, char *argv[]) {

    int sfd, cfd;
    ssize_t numRead = 0, totalRead = 0;
    struct sockaddr_un addr;
    char *p;
    char buf1[MSG_SIZE + 1], buf2[MSG_SIZE + 1];

    memset(buf1, 0, sizeof(buf1));
    memset(buf2, 0, sizeof(buf2));

    if ((sfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        exitErr("error creating socket");
    }

    if (remove(SV_SOCK_PATH) == -1 && errno != ENOENT) {
        exitErr("error removing socket");
    }

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SV_SOCK_PATH, sizeof(addr.sun_path) - 1);

    if (bind(sfd, (struct sockaddr *) &addr, sizeof(struct sockaddr_un)) == -1) {
        exitErr("error binding socket");
    }

    if (listen(sfd, BACKLOG) == -1) {
        exitErr("error listening on socket");
    }


    for (;;) {

        if ((cfd = accept(sfd, NULL, 0)) == -1) {
            exitErr("error accepting connection");
        }

        fprintf(stderr, "connection accepted\n");

        for (;;) {
            if ((numRead = read(cfd, buf1 + totalRead, MSG_SIZE - totalRead)) == -1) {
                exitErr("error reading from socket");
            }

            if (numRead == 0) {
                fprintf(stderr, "Client disconnected. Shutting down.\n");
                exit(EXIT_SUCCESS);
            }

            totalRead += numRead;

            fprintf(stderr, "read.\n");

            p = strstr(buf1, CRLF);

            if (p != NULL) {
                fprintf(stderr, "Full message read, parsing\n");
                fprintf(stderr, "Message length: %ld\n", p + 2 - buf1);
                for (char * i = buf1; i < p; i++) {
                    fprintf(stderr, "%d ", *i);
                }
                fprintf(stderr, "\n");
                memset(buf1, 0, sizeof(buf1));
                totalRead = 0;
//            p - buf1;
//            memmove(buf1, p + 2, )
            } else {
                if (totalRead == MSG_SIZE) {
                    if (write(cfd, "error parsing message", sizeof("error parsing message")) == -1) {
                        exitErr("error writing error message");
                    }
                    totalRead = 0;
                    memset(buf1, 0, sizeof(buf1));
                } else {
                    fprintf(stderr, "Not full message read, will read more. Total size: %ld\n", totalRead);
                }
//            if ((numRead = read(cfd, buf1 + numRead, sizeof(buf1) - numRead)) == -1) {
//                exitErr("error reading from socket");
//            }
            }

//        fflush(stdout);
        }



    }

//    if (listen())
    return 0;
}