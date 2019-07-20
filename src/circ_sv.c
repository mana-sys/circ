#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
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
#include "context.h"
#include "handlers.h"
#include "hashtable.h"
#include "irc.h"
#include "log.h"
#include "msgtok.h"
#include "parser.h"
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

struct hashtable_table *g_nicknames;

struct conn_params {
    int cfd;
    struct sockaddr_in addr;
    socklen_t len;
};

struct conn_info {
    bool registered, gotNickname, gotUsername;
    char nickname[NICKNAME_SIZE + 1];
    char username[512];
    char fullname[512];
};

static void sigHandler(int sig)
{
    if (sig == SIGINT) {
        circlog(L_INFO, "SIGINT received; shutting down server.");
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
    struct irc_message message;
    struct conn_params params;
    struct context_client client;
    char storeBuf[MSG_SIZE], buf[MSG_SIZE], outBuf[MSG_SIZE];
    char hostname[100];
    int readRes;

    params.cfd = ((struct conn_params *) arg)->cfd;
    params.len = ((struct conn_params *) arg)->len;
    memcpy(&params.addr, &((struct conn_params *) arg)->addr, sizeof(struct sockaddr_in));

    free(arg);

    getnameinfo((struct sockaddr *) &params.addr, params.len, hostname, 100, NULL, 0, 0);
    circlog(L_INFO, "Established connection with %s.", hostname);

    memset(&client, 0, sizeof(struct context_client));
    memset(buf, 0, MSG_SIZE);

    client.fd = params.cfd;

    for (;;) {
        readRes = read_message(params.cfd, buf, storeBuf, &storeTotalRead, &storeDiscardNext);
        if (readRes == -1) {
            perror("read_message()");
            pthread_exit((void *) 1);
        } else if (readRes == 0) {
            circlog(L_INFO, "Client closed connection. Closing socket.");
            if (close(params.cfd) == -1) {
                perror("close()");
                pthread_exit((void *) 1);
            }
            pthread_exit((void *) 0);
        }

        memset(&message, 0, sizeof(struct irc_message));
        parse_message(buf, &message);
        handle_message(&client, NULL, &message, outBuf);
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

static void printUsage()
{
    fprintf(stderr, "Usage: circ_sv -h hostname -p port -l level\n");
}

int main(int argc, char *argv[]) {

    int opt;
    char *hostname = NULL, *port = NULL, *loglevelstr = NULL;
    loglevel_t loglevel;

    while ((opt = getopt(argc, argv, ":h:p:l:")) != -1) {
        switch(opt) {
            case 'h':
                hostname = optarg;
                break;
            case 'p':
                port = optarg;
                break;
            case 'l':
                loglevelstr = optarg;
                break;
            case ':':
                fprintf(stderr, "Missing argument for option -%c.\n", optopt);
                printUsage();
                exit(EXIT_FAILURE);
            case '?':
                fprintf(stderr, "Unrecognized option -%c.\n", optopt);
                printUsage();
                exit(EXIT_FAILURE);
            default:
                fprintf(stderr, "Unexpected case in switch().\n");
                exit(EXIT_FAILURE);
        }
    }

    fprintf(stderr, "hostname is %s\n", hostname);
    fprintf(stderr, "port is %s\n", port);
    fprintf(stderr, "loglevel is %s\n", loglevelstr);

    if (parse_loglevel(loglevelstr) == -1) {
        fprintf(stderr, "Invalid argument for option -l specified.\n");
        printUsage();
        exit(EXIT_FAILURE);
    }

    set_loglevel(L_TRACE);

    circlog(L_TRACE, "Starting server...");

    int s;
    socklen_t len;
    pthread_t t;
    struct sockaddr_in svaddr;
    struct conn_params *params;
    char errbuf[50];


    if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        strerror_r(errno, errbuf, 50);
        circlog(L_CRITICAL, "socket(): %s\n", errbuf);
        exit(EXIT_FAILURE);
    }

    memset(&svaddr, 0, sizeof(struct sockaddr_in));
    svaddr.sin_family = AF_INET;
    svaddr.sin_port = htons(PORT_NUM);
    if (inet_pton(AF_INET, "0.0.0.0", &svaddr.sin_addr) == -1) {
        fprintf(stderr, "Error converting\n");
    }

    if (bind(sfd, (struct sockaddr *) &svaddr, sizeof(struct sockaddr_in)) == -1) {
        strerror_r(errno, errbuf, 50);
        circlog(L_CRITICAL, "error on socket(): %s", errbuf);
        exit(EXIT_FAILURE);
    }

    if (listen(sfd, BACKLOG) == -1) {
        exitErr("error listening on socket");
    }

    if (signal(SIGINT, sigHandler) == SIG_ERR) {
        perror("signal()");
        exit(EXIT_FAILURE);
    }


    for (;;) {
        params = malloc(sizeof(struct conn_params));
        params->len = sizeof(struct sockaddr_in);
        if ((params->cfd = accept(sfd, (struct sockaddr *) &params->addr, &params->len)) == -1) {
            exitErr("error accepting connection");
        }


        s = pthread_create(&t, NULL, handle_conn, params);
        if (s != 0) {
            circlog(L_ERROR, "Failed to create new thread for incoming connection.");
            continue;
        }
        circlog(L_INFO, "Created new thread for connection.");

        pthread_detach(t);
    }
}