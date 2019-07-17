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
#include "msgtok.h"

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
    ssize_t numRead, totalRead = 0, numExtraBytes;
    size_t toklen;
    struct circ_msg msg;
    struct conn_info ci;
    struct conn_params params;
    char buf[MSG_SIZE];
    char *msg_end, *tok;
    int result;

    params.cfd = ((struct conn_params *) arg)->cfd;
    memcpy(&params.addr, &((struct conn_params *) arg)->addr, sizeof(struct sockaddr_in));

    free(arg);

    memset(&ci, 0, sizeof(struct conn_info));
    memset(buf, 0, MSG_SIZE);

    for (;;) {
        numRead = read(params.cfd, buf + totalRead, MSG_SIZE - totalRead);
        if (numRead == -1) {
            perror("read()");
            pthread_exit((void *) 1);
        }

        if (numRead == 0) {
            return 0;
        }

        totalRead += numRead;

        // Check if the buffer currently has CRLF (a full message).
        // If it does, parse the message. If not, we check if the buffer
        // is full. If the buffer is not full, we request more input. If the
        // buffer is full, we have received an incorrect message, and will
        // discard the contents of the buffer.
        if ((msg_end = strstr(buf, CRLF)) == NULL) {
            if (totalRead == MSG_SIZE) {
                fprintf(stderr, "Invalid message in full buffer. Discarding buffer contents.\n");
                memset(buf, 0, MSG_SIZE);
                totalRead = 0;
            } else {
                fprintf(stderr, "Waiting for more input... (totalRead = %ld)\n", totalRead);
                continue;
            }
        }

        numExtraBytes = totalRead - ((msg_end + 2) - buf);

        // Parse message. After parsing message, move the bytes after CRLF to the front of the buffer,
        // and zero out the rest of the buffer.

        // parse message here
        result = parse_msg(buf, &msg);
        if (result == 0) {
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

//        printf("parsing message\n");
//        tok = msgtok(buf, &toklen);
//        if (tok != NULL && toklen != 0) {
//            if (strncasecmp(tok, NICK, toklen) == 0) {
//                fprintf(stderr, "Got NICK statement\n");
//            } else if (strncasecmp(tok, USER, toklen) == 0) {
//                fprintf(stderr, "Got USER statement\n");
//            }
//        } else {
//            fprintf(stderr, "failed to parse\n");
//        }


         // Move contents of next message to the front of the buffer, and zero out the rest.
        memmove(buf, msg_end + 2, numExtraBytes);
        memset(buf + numExtraBytes, 0, MSG_SIZE - numExtraBytes);

        // Currently, have numExtraBytes in the buffer.
        totalRead = numExtraBytes;
    }
}

static char * strtok_no_replace_ptr;

char * strtok_no_replace(char * str, char delimiter, size_t *len)
{
    if (str == NULL) {
        if (strtok_no_replace_ptr == NULL) {
            return NULL;
        }
        str = strtok_no_replace_ptr;
    }
    char *i;
    for (i = str; *i != '\0'; i++) {
        if (*i == delimiter) {
            *len = i - str;
            strtok_no_replace_ptr = i + 1;
            return str;
        }
    }
    strtok_no_replace_ptr = NULL;
    return str;
}



void exitErr(const char * msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

//int parse_user_cmd(char *username, char *fullname)
//{
//    size_t toklen;
//    char *tok = msgtok(NULL, &toklen);
//
//
//
//    if (tok == NULL || toklen == 0) {
//        return ERR_NEEDMOREPARAMS;
//    }
//
//    strncpy(username, tok, toklen);
//    username[toklen + 1] = '\0';
//
//    if (msgtok(NULL, &toklen) == NULL) {
//        return ERR_NEEDMOREPARAMS;
//    }
//
//    if (msgtok(NULL, &toklen) == NULL) {
//        return ERR_NEEDMOREPARAMS;
//    }
//
//    if ((tok = msgtok(NULL, &toklen)) == NULL) {
//        return ERR_NEEDMOREPARAMS;
//    }
//
//    strncpy(fullname, tok, toklen);
//    fullname[toklen + 1] = '\0';
//
//    return 0;
//}

//int parse_nick_cmd(char *nickname)
//{
//    size_t toklen;
//    char *tok = msgtok(NULL, &toklen);
//
//    // If no nickname parameter given, then reply with
//    // ERR_NONICKNAMEGIVEN
//    if (tok == NULL || toklen == 0) {
//        return ERR_NONICKNAMEGIVEN;
//    } else if (toklen > NICKNAME_SIZE) {
//        return ERR_ERRONEUSNICKNAME;
//    }
//
//    // Copy nickname parameter into nickname buffer
//    strncpy(nickname, tok, toklen);
//    nickname[toklen + 1] = '\0';
//
//    return 0;
//}

// strategy
// - try to read up to 512 bytes from buffer
// - check message for \r\n sequence. If it exists, we have read an entire message and can parse.
// - if not exists, need to try to read (512 - numRead) bytes from the buffer. If the substring \r\n is not
// - found here, then that's an error. Otherwise, we parse the message, up to the \r\n. After parsing the message,
// - take the rest of the bytes and copy them to the front of the buffer

int main(int argc, char *argv[]) {

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

//        for (;;) {
//
//
//            if ((numRead = read(cfd, buf1 + totalRead, MSG_SIZE - totalRead)) == -1) {
//                exitErr("error reading from socket");
//            }
//
//            if (numRead == 0) {
//                fprintf(stderr, "Client disconnected. Shutting down.\n");
//                exit(EXIT_SUCCESS);
//            }
//
//            totalRead += numRead;
//
//            fprintf(stderr, "read.\n");
//
//            p = strstr(buf1, CRLF);
//
//            // registration steps:
//            // if NICK received first
//
//            if (p != NULL) {
//
//                fprintf(stderr, "Full message read, parsing\n");
//
//                tok = msgtok(buf1, &toklen);
//                if (tok != NULL && toklen != 0) {
//                    if (strncasecmp(tok, "NICK", toklen) == 0) {
//                        fprintf(stderr, "Got NICK statement\n");
//
//                        parse_result = parse_nick_cmd(nick);
//                        if (parse_result == ERR_NONICKNAMEGIVEN) {
//                            sprintf(reply, "%d %s %s\r\n", ERR_NONICKNAMEGIVEN, strlen(nick) ? nick : "*", ":No nickname given");
//                            printf("%s", reply);
//                            if (write(cfd, reply, strlen(reply)) != strlen(reply)) {
//                                fprintf(stderr, "error sending reply\n");
//                            }
//                        } else {
//                            nickReceived = true;
//                        }
//
//                    } else if (strncasecmp(tok, "USER", toklen) == 0) {
//
//                        fprintf(stderr, "Got USER statement\n");
//
//                        if (nickReceived) {
//                            if (!registered) {
//                                if (parse_user_cmd(username, fullname) == 0) {
//                                    fprintf(stderr, "Parsed USER command successfully\n");
//                                    fprintf(stderr, "Username: %s, Full name: %s\n", username, fullname);
//                                    registered = true;
//                                    sprintf(reply, "001 %s :Welcome to the Internet Relay Network %s!\r\n", nick, nick);
//                                    if (write(cfd, reply, strlen(reply)) != strlen(reply)) {
//                                        fprintf(stderr, "error sending reply\n");
//                                    }
//                                } else {
//                                    sprintf(reply, "%d %s %s %s\r\n", ERR_NEEDMOREPARAMS, "*", "USER", ":Not enough parameters");
//                                    if (write(cfd, reply, strlen(reply)) != strlen(reply)) {
//                                        fprintf(stderr, "error sending reply\n");
//                                    }
//                                    printf("%s", reply);
//                                }
//                            } else {
//                                sprintf(reply, "%d %s %s %s\r\n", ERR_ALREADYREGISTRED, nick, "USER", ":Unauthorized command (already registered)");
//                                if (write(cfd, reply, strlen(reply)) != strlen(reply)) {
//                                    fprintf(stderr, "error sending reply\n");
//                                }
//                                printf("%s", reply);
////                                fprintf(stderr, "already registered");
//                            }
//                        } else {
//                            fprintf(stderr, "nick must be sent first");
//                        }
//                    } else {
//                        fprintf(stderr, "Unknown statement type\n");
//                    }
//                } else {
//                    fprintf(stderr, "error parsing message\n");
//                }
//                memset(buf1, 0, sizeof(buf1));
//                totalRead = 0;
//            } else {
//                if (totalRead == MSG_SIZE) {
//                    if (write(cfd, "error parsing message", sizeof("error parsing message")) == -1) {
//                        exitErr("error writing error message");
//                    }
//                    totalRead = 0;
//                    memset(buf1, 0, sizeof(buf1));
//                } else {
//                    fprintf(stderr, "Not full message read, will read more. Total size: %ld\n", totalRead);
//                }
//            }
//        }
    }
}