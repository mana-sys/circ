#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define BACKLOG 5
#define MSG_SIZE 512
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

    int sfd;
    ssize_t numRead;
    struct sockaddr_un addr;
    char *p;
    char buf1[MSG_SIZE + 1], buf2[MSG_SIZE + 1];

    memset(buf1, 0, sizeof(buf1));
    memset(buf2, 0, sizeof(buf2));

    if ((sfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        exitErr("error creating socket");
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
        if ((numRead = read(sfd, buf1, sizeof(buf1))) == -1) {
            exitErr("error reading from socket");
        }

        p = strstr(buf1, "\r\n");

        if (p != NULL) {
            printf("Full message read, parsing");
        } else {
            printf("Not full message read, will read more");
        }

    }

//    if (listen())
    return 0;
}