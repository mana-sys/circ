#include <stdio.h>

#include "msgtok.h"
#include "parser.h"
#include "codes.h"

#define NICK_MAX_SIZE 9

static int parse_message_nick    (struct irc_message_nick    *message, char *saveptr);
static int parse_message_user    (struct irc_message_user    *message, char *saveptr);
static int parse_message_privmsg (struct irc_message_privmsg *message, char *saveptr);



static int parse_message_nick(struct irc_message_nick *message, char *saveptr)
{
    size_t toklen;
    char *tok;

    // Parse the <nickname> parameter.
    tok = msgtok_r(NULL, &toklen, &saveptr);
    if (tok == NULL || toklen == 0) {
        return ERR_NONICKNAMEGIVEN;
    } else if (toklen > NICK_MAX_SIZE) {
        return ERR_ERRONEUSNICKNAME;
    }

    message->nick = tok;
    return 0;

}

static int parse_message_user(struct irc_message_user *message, char *saveptr)
{
    size_t toklen;
    int err;
    char * tok;

    // Parse the <user> parameter.
    tok = msgtok_r(NULL, &toklen, &saveptr);

    if (tok == NULL || toklen == 0) {
        return ERR_NEEDMOREPARAMS;
    }

    message->username = tok;

    // Skip the <mode> parameter.
    if ((msgtok_r(NULL, &toklen, &saveptr)) == NULL) {
        return ERR_NEEDMOREPARAMS;
    }

    // Skip the <user> parameter.
    if ((msgtok_r(NULL, &toklen, &saveptr)) == NULL) {
        return ERR_NEEDMOREPARAMS;
    }

    // Parse the <realname> parameter.
    if ((msgtok_r(NULL, &toklen, &saveptr)) == NULL) {
        return ERR_NEEDMOREPARAMS;
    }

    message->fullname = tok;
    return 0;

}

int parse_message(char *buf, struct irc_message *message)
{
    size_t toklen;
    char *tok, *saveptr;

    tok = msgtok_r(buf, &toklen, &saveptr);
    if (tok != NULL && toklen != 0) {
        if (strcmp(tok, "NICK") == 0) {
            return parse_message_nick(&message->message.nick, saveptr);
        } else if (strcmp(tok, "USER") == 0) {
            return parse_message_user(&message->message.user, saveptr);
        }
    }

    fprintf(stderr, "Error parsing message: invalid message buffer.\n");

    return -1;
}
