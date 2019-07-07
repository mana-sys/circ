#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SV_SOCK_PATH "/tmp/circ.sock"

#define MSG_SIZE 512

void exitErr(const char * msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    int sfd, cfd;
    char buf[512];
    struct sockaddr_un addr;

    ssize_t numRead;
    const char * msg = "hello there\r\n";

    if ((sfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        exitErr("create socket");
    }

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SV_SOCK_PATH, sizeof(addr.sun_path) - 1);

    if (connect(sfd, (struct sockaddr *) &addr, sizeof(struct sockaddr_un)) == -1) {
        exitErr("error establishing connection");
    }

    while ((numRead = read(STDIN_FILENO, buf, MSG_SIZE - 2)) > 0) {
        buf[numRead - 1] = '\r';
        buf[numRead] = '\n';
        if (write(sfd, buf, numRead + 1) != numRead + 1) {
            exitErr("error writing");
        }
        fprintf(stderr, "Wrote %ld characters.", numRead + 1);
    }

//    if ((numRead = write(sfd, msg, strlen(msg))) == -1) {
//        exitErr("error writing");
//    }

//    printf("Wrote %ld bytes.\n", numRead);
    if (close(sfd) == -1) {
        exitErr("error closing");
    }
    

}