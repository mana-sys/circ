//
// Created by manaxlalanes on 7/18/19.
//

#ifndef REPLIES_H
#define REPLIES_H

#define STR_ERR_NONICKNAMEGIVEN ":No nickname given"
#define STR_ERR_NICKNAMEINUSE ":Nickname is already in use"
#define STR_ERR_NEEDMOREPARAMS ":Not enough parameters"
#define STR_ERR_NOTREGISTERED ":You have not registered"
#define STR_ERR_ALREADYREGISTERED ":You may not reregister"
#define STR_ERR_UNKNOWNCOMMAND ":Unknown command"

#include "context.h"

int replyf_rpl_welcome(struct context_client *client, char *buf)
{
    return sprintf(buf, "%03d %s Welcome to the Internet Relay Network %s!%s@%s\r\n", RPL_WELCOME,
            client->nickname, client->nickname, client->username, client->hostname);
}

int replyf_err_nonicknamegiven(struct context_client *client, struct context_server *server, char *buf)
{
    return sprintf(buf, "%d %s %s\r\n",
            ERR_NONICKNAMEGIVEN, client->receivedNick ? client->nickname : "*", STR_ERR_NONICKNAMEGIVEN);
}

int replyf_err_nicknameinuse(struct context_client *client, struct context_server *server, char *buf,
        const char *failedNick)
{
    return sprintf(buf, "%d %s %s %s\r\n",
                   ERR_NONICKNAMEGIVEN, client->receivedNick ? client->nickname : "*",
                   failedNick, STR_ERR_NICKNAMEINUSE);
}

int replyf_err_needmoreparams(struct context_client *client, const char *failedCommand, char *buf)
{
    return sprintf(buf, "%d %s %s %s\r\n", ERR_NEEDMOREPARAMS, client->receivedNick ? client->nickname : "*",
            failedCommand, STR_ERR_NEEDMOREPARAMS);
}

int replyf_err_notregistered(struct context_client *client, char *buf)
{
    return sprintf(buf, "%d %s %s", 451, client->receivedNick ? client->nickname : "*", STR_ERR_NOTREGISTERED);
}

int replyf_err_alreadyregistered(struct context_client *client, char *buf)
{
    return sprintf(buf, "%d %s %s\r\n", 462, client->nickname, STR_ERR_ALREADYREGISTERED);
}

int replyf_err_unknowncommand(struct context_client *client, const char *unknownCommand, char *buf)
{
    return sprintf(buf, "%d %s %s %s\r\n", 421, client->receivedNick ? client->nickname : "*",
            unknownCommand, STR_ERR_UNKNOWNCOMMAND);
}

#endif
