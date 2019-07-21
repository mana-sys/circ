#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
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
    int sfd, nfds, ready;
    char buf[512], replybuf[512];
//    struct sockaddr_un addr;
    struct sockaddr_in addr;
    fd_set readfds;

    ssize_t numRead;
    const char * msg = "hello there\r\n";

    if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        exitErr("create socket");
    }

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(50002);
//    strncpy(addr.sun_path, SV_SOCK_PATH, sizeof(addr.sun_path) - 1);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    if (connect(sfd, (struct sockaddr *) &addr, sizeof(struct sockaddr_in)) == -1) {
        exitErr("error establishing connection");
    }

    if (sfd < STDIN_FILENO)
        nfds = STDIN_FILENO + 1;
    else
        nfds = sfd + 1;

    for (;;) {
        FD_ZERO(&readfds);

        FD_SET(STDIN_FILENO, &readfds);
        FD_SET(sfd, &readfds);

        ready = select(nfds, &readfds, NULL, NULL, NULL);

        if (ready == -1) {
            perror("select()");
            exit(EXIT_FAILURE);
        }

        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            numRead = read(STDIN_FILENO, buf, MSG_SIZE - 2);
            if (numRead == 0) {
                printf("Quitting...\n");
                if (close(sfd) == -1) {
                    perror("close()");
                    exit(EXIT_FAILURE);
                }
                exit(EXIT_SUCCESS);
            }
            buf[numRead - 1] = '\r';
            buf[numRead] = '\n';
            if (write(sfd, buf, numRead + 1) != numRead + 1) {
                exitErr("error writing");
            }
        }

        if (FD_ISSET(sfd, &readfds)) {
            numRead = read(sfd, buf, sizeof(replybuf));
            if (numRead == 0) {
                printf("Server closed connection - exiting...\n");
                if (close(sfd) == -1) {
                    perror("close()");
                    exit(EXIT_FAILURE);
                }
                exit(EXIT_SUCCESS);
            }
            if (write(STDOUT_FILENO, buf, numRead) != numRead) {
                exitErr("write()");
            }
        }
    }

    while ((numRead = read(STDIN_FILENO, buf, MSG_SIZE - 2)) > 0) {
        buf[numRead - 1] = '\r';
        buf[numRead] = '\n';
        if (write(sfd, buf, numRead + 1) != numRead + 1) {
            exitErr("error writing");
        }
        fprintf(stderr, "Wrote %ld characters.\n", numRead + 1);
    }

//    if ((numRead = write(sfd, msg, strlen(msg))) == -1) {
//        exitErr("error writing");
//    }

//    printf("Wrote %ld bytes.\n", numRead);
    if (close(sfd) == -1) {
        exitErr("error closing");
    }
    

}