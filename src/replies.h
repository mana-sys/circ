//
// Created by manaxlalanes on 7/18/19.
//

#ifndef REPLIES_H
#define REPLIES_H

#define STR_ERR_NONICKNAMEGIVEN ":No nickname given"

#include "context.h"

int replyf_err_nonicknamegiven(struct context_client *client, struct context_server *server, char *buf)
{
    return sprintf(buf, "%d %s %s\r\n",
            ERR_NONICKNAMEGIVEN, client->receivedNick ? client->nickname : "*", STR_ERR_NONICKNAMEGIVEN);
}



#endif
