//
// Created by manaxlalanes on 7/18/19.
//

#ifndef REPLIES_H
#define REPLIES_H

#define FMT_RPL_MOTDSTART           "%d %s :- %s Message of the day - \r\n"
#define FMT_RPL_MOTD                "%d %s :- %s\r\n"
#define FMT_RPL_ENDOFMOTD           "%d %s :End of MOTD command\r\n"

/*
 * LUSERS replies.
 */
#define FMT_RPL_LUSERCLIENT             "251 %s :There are %d users and %d services on %d servers\r\n"
#define FMT_RPL_LUSEROP                 "252 %s %d :operator(s) online\r\n"
#define FMT_RPL_LUSERUNKNOWN            "253 %s %d :unknown connection(s)\r\n"
#define FMT_RPL_LUSERCHANNELS           "254 %s %d :channels formed\r\n"
#define FMT_RPL_LUSERME                 "255 %s :I have %d clients and %d servers\r\n"

#define FMT_ERR_NORECIPIENT         "%d %s :No recipient given (%s)\r\n"
#define FMT_ERR_NOTEXTTOSEND        "%d %s :No text to send\r\n"

#define FMT_MSG_PRIVMSG             "%s!%s@%s PRIVMSG %s :%s\r\n"


/*
 * WHOIS replies.
 */
#define FMT_RPL_WHOISUSER           "311 %s %s %s %s * :%s\r\n"
#define FMT_RPL_WHOISSERVER         "312 %s %s %s :%s\r\n"
#define FMT_RPL_ENDOFWHOIS          "318 %s %s :End of WHOIS list\r\n"


/*
 * JOIN replies.
 */
#define FMT_MSG_JOIN                ":%s!%s@%s JOIN %s\r\n"
#define FMT_RPL_TOPIC               ":%s 332 %s %s :%s\r\n"
#define FMT_RPL_NAMREPLY_PARTIAL    ":%s 353 %s %s :"
#define FMT_RPL_ENDOFNAMES          ":%s 366 %s %s :End of NAMES list\r\n"


/*
 * LIST replies.
 */
#define FMT_RPL_LISTSTART           ":%s 321 %s Channel :Users Name\r\n"
#define FMT_RPL_LIST                ":%s 322 %s %s %d :%s\r\n"
#define FMT_RPL_LISTEND                 ":%s 323 %s :End of /LIST\r\n"

/*
 * Channel-related replies.
 */
#define FMT_ERR_NOSUCHCHANNEL       ":%s 403 %s %s :No such channel\r\n"
#define FMT_ERR_NOTONCHANNEL        ":%s 442 %s %s :You're not on that channel\r\n"

#define STR_ERR_NOSUCHNICK          ":No such nick/channel"
#define STR_ERR_NOMOTD              ":MOTD File is missing"
#define STR_ERR_NONICKNAMEGIVEN     ":No nickname given"
#define STR_ERR_NICKNAMEINUSE       ":Nickname is already in use"
#define STR_ERR_NEEDMOREPARAMS      ":Not enough parameters"
#define STR_ERR_NOTREGISTERED       ":You have not registered"
#define STR_ERR_ALREADYREGISTERED   ":You may not reregister"
#define STR_ERR_UNKNOWNCOMMAND      ":Unknown command"

#include "client.h"

int Reply_RplWelcome           (client_s *, char *);
int Reply_RplMotdStart         (client_s *, const char *, char *);
int Reply_RplMotd              (client_s *, const char *, char *);
int Reply_RplEndOfMotd         (client_s *, char *);
int Reply_ErrNoSuchNick        (client_s *, const char *, char *);
int Reply_ErrNoMotd            (client_s *, char *);


/*
 * PRIVMSG replies.
 */
int Reply_ErrNoRecipient       (client_s *, const char *, char *);
int Reply_ErrNoTextToSend      (client_s *, char *);


/*
 * LUSERS replies.
 */
int Reply_RplLUserClient   (client_s *client, server_s *server, char *response);
int Reply_RplLUserOp       (client_s *client, server_s *server, char *response);
int Reply_RplLUserUnknown  (client_s *client, server_s *server, char *response);
int Reply_RplLUserChannels (client_s *client, server_s *server, char *response);
int Reply_RplLUserMe       (client_s *client, server_s *server, char *response);

/*
 * WHOIS replies.
 */
int Reply_RplWhoIsUser   (client_s *client, client_s *queried, char response[static IRC_MSG_SIZE]);
int Reply_RplWhoIsServer (client_s *client, client_s *queried, char response[static IRC_MSG_SIZE]);
int Reply_RplEndOfWhoIs  (client_s *client, client_s *queried, char response[static IRC_MSG_SIZE]);


/*
 * JOIN replies.
 */
int Reply_RplTopic(client_s *client, server_s *server, channel_s *channel, char response[static IRC_MSG_SIZE]);
int Reply_RplNamReply(client_s *client, server_s *server, channel_s *channel, char response[static IRC_MSG_SIZE]);
int Reply_RplNamReplyAll(client_s *client, server_s *server, channel_s *channel, GQueue *responses);
int Reply_RplEndOfNames(client_s *client, server_s *server, channel_s *channel, char response[static IRC_MSG_SIZE]);

/*
 * LIST replies.
 */
int Reply_RplListStart(client_s *client, server_s *server, char response[static IRC_MSG_SIZE]);
int Reply_RplList(client_s *client, server_s *server, channel_s *channel, char response[static IRC_MSG_SIZE]);
int Reply_RplListEnd(client_s *client, server_s *server, char response[static IRC_MSG_SIZE]);



/*
 * Channel-related replies.
 */
int Reply_ErrNoSuchChannel(client_s *client, server_s *server, const char *channel_name, char response[static IRC_MSG_SIZE]);
int Reply_ErrNotOnChannel(client_s *client, server_s *server, channel_s *channel, char response[static IRC_MSG_SIZE]);




int Reply_ErrNoNicknameGiven   (client_s *, char *);
int Reply_ErrNicknameInUse     (client_s *, const char *, char *);
int Reply_ErrNeedMoreParams    (client_s *, const char *, char *);
int Reply_ErrNotRegistered     (client_s *, char *);
int Reply_ErrAlreadyRegistered (client_s *, char *);
int Reply_ErrUnknownCommand    (client_s *, const char *, char *);
int Reply_Pong                 (const char *, char *);




int Message_Privmsg(client_s *, const char *msgtarget, const char *contents, char *response);
int Format_MessageJoin(const client_s *client, const char *channel, char *response);


#endif
