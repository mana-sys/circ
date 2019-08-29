//
// Created by mana on 8/20/19.
//
#include <stdio.h>

#include "codes.h"
#include "replies.h"

#define FORMAT_RPL_WELCOME "%03d %s :Welcome to the Internet Relay Network %s!%s@%s\r\n"
#define NICK_OR_STAR(client) (client->receivedNick ? client->nickname : "*")

int Reply_RplWelcome           (client_s * client, char *response)
{
    return sprintf(response, FORMAT_RPL_WELCOME, RPL_WELCOME, client->nickname,
            client->nickname, client->username, client->hostname);
}

int Reply_ErrNoSuchNick (client_s *client, const char *nick, char *response)
{
   return sprintf(response, "%d %s %s %s\r\n", ERR_NOSUCHNICK, NICK_OR_STAR(client), nick, STR_ERR_NOSUCHNICK);
}

int Reply_ErrNoNicknameGiven   (client_s *client, char *response)
{
    return sprintf(response, "%d %s %s\r\n",
                   ERR_NONICKNAMEGIVEN, client->receivedNick ? client->nickname : "*", STR_ERR_NONICKNAMEGIVEN);
}

int Reply_ErrNicknameInUse     (client_s *client, const char *failedNick, char *response)
{
    return sprintf(response, "%d %s %s %s\r\n",
                   ERR_NICKNAMEINUSE, client->receivedNick ? client->nickname : "*",
                   failedNick, STR_ERR_NICKNAMEINUSE);
}

int Reply_ErrNeedMoreParams    (client_s *client, const char *failedCommand, char *response)
{
    return sprintf(response, "%d %s %s %s\r\n", ERR_NEEDMOREPARAMS, client->receivedNick ? client->nickname : "*",
                   failedCommand, STR_ERR_NEEDMOREPARAMS);
}

int Reply_ErrNotRegistered     (client_s *client, char *response)
{
    return sprintf(response, "%d %s %s", 451, client->receivedNick ? client->nickname : "*", STR_ERR_NOTREGISTERED);

}

int Reply_ErrAlreadyRegistered (client_s *client, char *response)
{
    return sprintf(response, "%d %s %s\r\n", 462, client->nickname, STR_ERR_ALREADYREGISTERED);

}

int Reply_ErrUnknownCommand    (client_s *client, const char *unknownCommand, char *response)
{
    return sprintf(response, "%d %s %s %s\r\n", 421, client->receivedNick ? client->nickname : "*",
                   unknownCommand, STR_ERR_UNKNOWNCOMMAND);
}

int Reply_Pong (const char *hostname, char *response)
{
    return sprintf(response, "PONG %s\r\n", hostname);
}