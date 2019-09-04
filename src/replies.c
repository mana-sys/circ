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

int Reply_RplMotdStart(client_s * client, const char *server, char *response)
{
    return sprintf(response, FMT_RPL_MOTDSTART, RPL_MOTDSTART, NICK_OR_STAR(client), server);
}

int Reply_RplMotd(client_s * client, const char *part, char *response)
{
    return sprintf(response, FMT_RPL_MOTD, RPL_MOTD, NICK_OR_STAR(client), part);
}

int Reply_RplEndOfMotd (client_s *client, char *response)
{
    return sprintf(response, FMT_RPL_ENDOFMOTD, RPL_ENDOFMOTD, NICK_OR_STAR(client));
}

int Reply_ErrNoSuchNick (client_s *client, const char *nick, char *response)
{
   return sprintf(response, "%d %s %s %s\r\n", ERR_NOSUCHNICK, NICK_OR_STAR(client), nick, STR_ERR_NOSUCHNICK);
}

int Reply_ErrNoMotd (client_s *client, char *response)
{
    return snprintf(response, IRC_MSG_SIZE + 1, "%d %s %s\r\n",
            ERR_NOMOTD, NICK_OR_STAR(client), STR_ERR_NOMOTD);
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


int Reply_ErrNoRecipient(client_s *client, const char *command, char *response)
{
    return sprintf(response, FMT_ERR_NORECIPIENT, ERR_NORECIPIENT, NICK_OR_STAR(client), command);
}


int Reply_ErrNoTextToSend(client_s * client, char *response)
{
    return sprintf(response, FMT_ERR_NOTEXTTOSEND, ERR_NOTEXTTOSEND, NICK_OR_STAR(client));
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

int Message_Privmsg(client_s *client, const char *msgtarget, const char *contents, char *response)
{
    return sprintf(response, FMT_MSG_PRIVMSG, client->nickname, client->username, client->hostname,
            msgtarget, contents);
}
