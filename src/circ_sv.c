#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "circ.h"
#include "hashtable.h"
#include "log.h"
#include "msgtok.h"
#include "read_message.h"

#define BACKLOG 5
#define MSG_SIZE 512
#define PORT_NUM 50002

int sfd;

int write_err_erroneusnickname(int fd, char *err_nick) {
    char reply[MSG_SIZE];
    size_t reply_size;

    sprintf(reply, "%d %s %s\r\n", ERR_NONICKNAMEGIVEN, strlen(err_nick) ? err_nick : "*", ":No nickname given");
    reply_size = strlen(reply);

    if (write(fd, reply, reply_size) != reply_size) {
        return 1;
    }
    return 0;
}

struct conn_params {
    int cfd;
    struct sockaddr_in addr;
};

struct conn_info {
    bool registered;
    char nickname[NICKNAME_SIZE + 1];
    char username[512];
    char fullname[512];
};

static void sigHandler(int sig)
{
    if (sig == SIGINT) {
        printf("Caught SIGINT - shutting down server.\n");
        if (close(sfd) == -1) {
            perror("close()");
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    }
}

static void *handle_conn(void *arg)
{
    ssize_t storeTotalRead = 0;
    bool storeDiscardNext = false;
    struct circ_msg msg;
    struct conn_info ci;
    struct conn_params params;
    char storeBuf[MSG_SIZE], buf[MSG_SIZE];
    char *msg_end, *tok;
    int readRes, parseRes;

    params.cfd = ((struct conn_params *) arg)->cfd;
    memcpy(&params.addr, &((struct conn_params *) arg)->addr, sizeof(struct sockaddr_in));

    free(arg);

    memset(&ci, 0, sizeof(struct conn_info));
    memset(buf, 0, MSG_SIZE);

    for (;;) {
        readRes = read_message(params.cfd, buf, storeBuf, &storeTotalRead, &storeDiscardNext);
        if (readRes == -1) {
            perror("read_message()");
            pthread_exit((void *) 1);
        } else if (readRes == 0) {
            fprintf(stderr, "Reached EOF. Closing socket.\n");
            if (close(params.cfd) == -1) {
                perror("close()");
                pthread_exit((void *) 1);
            }
            pthread_exit((void *) 0);
        }

        // Parse message.
        parseRes = parse_msg(buf, &msg);
        if (parseRes == 0) {
            switch (msg.msgType) {
                case MSGUSER:
                    fprintf(stderr, "Got USER statement (username: %s, fullname: %s)\n",
                            msg.msgUser.username, msg.msgUser.fullname);
                    break;
                case MSGNICK:
                    fprintf(stderr, "Got NICK statement\n");
                    fprintf(stderr, "Nickname is %s\n", msg.msgNick.nick);
                    break;
                default:
                    fprintf(stderr, "Unknown/unsupported message type\n");
            }
        } else {
            fprintf(stderr, "Invalid message format.\n");
        }
    }
}

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



//    circlog(L_DEBUG, "Starting server...\n");

    int s;
    socklen_t len;
    pthread_t t;
    struct sockaddr_in svaddr;
    struct conn_params *params;


    if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        exitErr("error creating socket");
    }

    memset(&svaddr, 0, sizeof(struct sockaddr_in));
    svaddr.sin_family = AF_INET;
    svaddr.sin_port = PORT_NUM;
    if (inet_pton(AF_INET, "0.0.0.0", &svaddr.sin_addr) == -1) {
        fprintf(stderr, "Error converting\n");
    }

    if (bind(sfd, (struct sockaddr *) &svaddr, sizeof(struct sockaddr_in)) == -1) {
        exitErr("error binding socket");
    }

    if (listen(sfd, BACKLOG) == -1) {
        exitErr("error listening on socket");
    }

    if (signal(SIGINT, sigHandler) == SIG_ERR) {
        perror("signal()");
        exit(EXIT_FAILURE);
    }


    for (;;) {
        len = sizeof(struct sockaddr_in);
        params = malloc(sizeof(struct conn_params));
        if ((params->cfd = accept(sfd, (struct sockaddr *) &params->addr, &len)) == -1) {
            exitErr("error accepting connection");
        }

        fprintf(stderr, "connection accepted\n");

        s = pthread_create(&t, NULL, handle_conn, params);

        fprintf(stderr, "created thread\n");
        if (s != 0) {
            perror("pthread_create()");
            continue;
        }
        pthread_detach(t);
    }
}