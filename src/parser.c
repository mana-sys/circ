#include <stdio.h>

#include "msgtok.h"
#include "parser.h"
#include "codes.h"

#define NICK_MAX_SIZE 9

static int parse_message_nick    (struct irc_message *message, char *saveptr);
static int parse_message_user    (struct irc_message *message, char *saveptr);
static int parse_message_privmsg (struct irc_message *message, char *saveptr);

static int parse_message_nick(struct irc_message *message, char *saveptr)
{
    size_t toklen;
    char *tok;

    message->type = NICK;

    // Parse the <nickname> parameter.
    tok = msgtok_r(NULL, &toklen, &saveptr);
    if (tok == NULL || toklen == 0) {
        message->parse_err = ERR_NONICKNAMEGIVEN;
        return ERR_NONICKNAMEGIVEN;
    } else if (toklen > NICK_MAX_SIZE) {
        message->parse_err = ERR_ERRONEUSNICKNAME;
        return ERR_ERRONEUSNICKNAME;
    }

    message->message.nick.nick = tok;
    message->parse_err = 0;

    return 0;

}

static int parse_message_user(struct irc_message *message, char *saveptr)
{
    size_t toklen;
    int err;
    char * tok;

    message->type = USER;

    // Parse the <user> parameter.
    tok = msgtok_r(NULL, &toklen, &saveptr);

    if (tok == NULL || toklen == 0) {
        message->parse_err = ERR_NEEDMOREPARAMS;
        return ERR_NEEDMOREPARAMS;
    }

    message->message.user.username = tok;

    // Skip the <mode> parameter.
    if ((msgtok_r(NULL, &toklen, &saveptr)) == NULL) {
        message->parse_err = ERR_NEEDMOREPARAMS;
        return ERR_NEEDMOREPARAMS;
    }

    // Skip the <user> parameter.
    if ((msgtok_r(NULL, &toklen, &saveptr)) == NULL) {
        message->parse_err = ERR_NEEDMOREPARAMS;
        return ERR_NEEDMOREPARAMS;
    }

    // Parse the <realname> parameter.
    if ((msgtok_r(NULL, &toklen, &saveptr)) == NULL) {
        message->parse_err = ERR_NEEDMOREPARAMS;
        return ERR_NEEDMOREPARAMS;
    }

    message->message.user.fullname = tok;
    message->parse_err = 0;

    return 0;

}

int parse_message(char *buf, struct irc_message *message)
{
    size_t toklen;
    char *tok, *saveptr;

    tok = msgtok_r(buf, &toklen, &saveptr);
    if (tok != NULL && toklen != 0) {
        message->command = tok;
        if (strcmp(tok, "NICK") == 0) {
            return parse_message_nick(message, saveptr);
        } else if (strcmp(tok, "USER") == 0) {
            return parse_message_user(message, saveptr);
        } else {
            return 0;
        }
    }

    fprintf(stderr, "Error parsing message: invalid message buffer.\n");

    return -1;
}
