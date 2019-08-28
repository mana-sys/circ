//
// Created by manaxlalanes on 7/19/19.
//

#ifndef HANDLERS_H
#define HANDLERS_H

#include <glib.h>

#include "client.h"
#include "irc.h"

typedef bool (*handler1_t)(client_s *, server_s *, irc_message_s *, char *, size_t *);
typedef int  (*handler2_t)(client_s *, server_s *, irc_message_s *, GQueue *);

int handle_message1(client_s *client, server_s *server, irc_message_s *message, char *responseBuffer, size_t *len);

int Handler_HandleMessage (client_s *client, server_s *server, irc_message_s *message, GQueue *responses);

#endif
