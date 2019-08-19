//
// Created by manaxlalanes on 7/19/19.
//

#ifndef HANDLERS_H
#define HANDLERS_H

#include "client.h"
#include "context.h"
#include "irc.h"

typedef bool (*handler_t)(struct context_client *, struct context_server *, struct irc_message *, char *buf);

int handle_message(struct context_client *client, struct context_server *server, struct irc_message *message, char *buf);

int handle_message1(client_s *client, server_s *server, irc_message_s *message, char *responseBuffer);

#endif
