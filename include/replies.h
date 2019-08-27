//
// Created by manaxlalanes on 7/18/19.
//

#ifndef REPLIES_H
#define REPLIES_H

#define STR_ERR_NOSUCHNICK ":No such nick/channel"
#define STR_ERR_NONICKNAMEGIVEN ":No nickname given"
#define STR_ERR_NICKNAMEINUSE ":Nickname is already in use"
#define STR_ERR_NEEDMOREPARAMS ":Not enough parameters"
#define STR_ERR_NOTREGISTERED ":You have not registered"
#define STR_ERR_ALREADYREGISTERED ":You may not reregister"
#define STR_ERR_UNKNOWNCOMMAND ":Unknown command"

#include "client.h"
#include "context.h"

int Reply_RplWelcome           (client_s *, char *);
int Reply_ErrNoSuchNick        (client_s *, const char *, char *);
int Reply_ErrNoNicknameGiven   (client_s *, char *);
int Reply_ErrNicknameInUse     (client_s *, const char *, char *);
int Reply_ErrNeedMoreParams    (client_s *, const char *, char *);
int Reply_ErrNotRegistered     (client_s *, char *);
int Reply_ErrAlreadyRegistered (client_s *, char *);
int Reply_ErrUnknownCommand    (client_s *, const char *, char *);
int Reply_Pong                 (const char *, char *);

int replyf_rpl_welcome(struct context_client *client, char *buf);
int replyf_err_nonicknamegiven(struct context_client *client, struct context_server *server, char *buf);
int replyf_err_nicknameinuse(struct context_client *client, struct context_server *server, char *buf,
                             const char *failedNick);
int replyf_err_needmoreparams(struct context_client *client, const char *failedCommand, char *buf);
int replyf_err_notregistered(struct context_client *client, char *buf);
int replyf_err_alreadyregistered(struct context_client *client, char *buf);
int replyf_err_unknowncommand(struct context_client *client, const char *unknownCommand, char *buf);
#endif
