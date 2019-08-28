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
    char hostname[IRC_HOSTNAME_MAX + 1];
    char fullname[IRC_MSG_SIZE];
} client_s;

typedef struct server_s {
    GHashTable *nicks;      /* Map from nicks (string) to client IDs (int) */
    GHashTable *clients;    /* Map from client IDs (int) to clients (client_s *) */
    const char *hostname;   /* Pointer to the server's hostname. */
    const char *motd;
} server_s;

/**
 * Registers the specified client with the specified server.
 * @param client The client to register.
 * @param server The server with which the client will be registered.
 * @param response Output parameter returning the generated response from registration.
 * @return 0 on success, -1 on error
 */
void Client_TryRegister (client_s *client, server_s *server, char *response, size_t *len);

/**
 * Tries to change the given client's nickname as registered by the given server.
 * Fails if the specified nickname has already been taken by another client
 * on the server.
 * @param client The client whose nickname to change.
 * @param server The server with which the nickname change is to be registered.
 * @param nick The proposed new nickname.
 * @param response Buffer to hold any response.
 * @return 0 on success, -1 on error
 */
int Client_TryChangeNick (client_s *client, server_s *server, const char *nick, char *response, size_t *len);

/**
 * Sends the specified message to the specified client.
 * @param client The client to send the message to.
 * @param message The message to be sent.
 * @return 0 on success, -1 on error
 */
int Client_Send(client_s *client, const char *message);

/**
 * Tries to send the specified message to the client identified by the
 * given nickname.
 * @param server
 * @param nick
 * @param message
 * @param response
 * @param len
 * @return
 */
int Server_TrySend (server_s *server, client_s *from_client, const char *nick, const char *message, char *response, size_t *len);

/**
 * 
 * @param client
 * @param message
 * @return
 */
int client_change_nick(client_s *client, char *message);

#endif //CIRC_CLIENT_H
