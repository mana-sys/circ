//
// Created by manaxlalanes on 7/19/19.
//

#ifndef HANDLERS_H
#define HANDLERS_H

#include "context.h"
#include "irc.h"

int handle_message(struct context_client *client, struct context_server *server, struct irc_message *message, char *buf);

#endif
