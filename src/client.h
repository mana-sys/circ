#ifndef CLIENT_H
#define CLIENT_H

#include <glib.h>
#include <stdbool.h>
#include <stdlib.h>

#include "irc_constants.h"

typedef struct client_s {
    bool registered, receivedNick, receivedUser;
    int fd, clientId;
    char nickname[IRC_NICK_SIZE + 1];
    char username[IRC_MSG_SIZE];
    char hostname[50];
    char fullname[IRC_MSG_SIZE];
} client_s;

typedef struct server_s {
    GHashTable *nicks;      /* Map from nicks (string) to client IDs (int) */
    GHashTable *clients;    /* Map from client IDs (int) to clients (client_s *) */
} server_s;

/**
 * Registers the specified client with the specified server.
 * @param client The client to register.
 * @param server The server with which the client will be registered.
 * @param response Output parameter returning the generated response from registration.
 * @return 0 on success, -1 on error
 */
int client_register (client_s *client, server_s *server, char *response);

/**
 * Sends the specified message to the specified client.
 * @param client The client to send the message to.
 * @param message The message to be sent.
 * @return 0 on success, -1 on error
 */
int client_send(client_s *client, char *message);

/**
 * 
 * @param client
 * @param message
 * @return
 */
int client_change_nick(client_s *client, char *message);

#endif //CIRC_CLIENT_H
