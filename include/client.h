#ifndef CLIENT_H
#define CLIENT_H

#include <glib.h>
#include <stdbool.h>
#include <stdlib.h>

#include "channel.h"
#include "connection.h"
#include "irc_constants.h"
#include "response.h"
#include "server.h"


/*
 * Structure to hold client data.
 */
typedef struct client_s {
    bool        registered, receivedNick, receivedUser;
    int         clientId;
    conn_s      conn;
    server_s *  server;
    char        nickname[IRC_NICK_SIZE + 1];
    char        username[IRC_MSG_SIZE];
    char        hostname[IRC_HOSTNAME_MAX + 1];
    char        fullname[IRC_MSG_SIZE];
    bool        away;
    char        away_message[IRC_MSG_SIZE + 1];
} client_s;


typedef int client_id_t;


/**
 * Registers the specified client with the specified server.
 * @param client The client to register.
 * @param server The server with which the client will be registered.
 * @param response Output parameter returning the generated response from registration.
 * @return 0 on success, -1 on error
 */
void Client_TryRegister (client_s *client, server_s *server, response_s **response);


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
int Client_TryChangeNick (client_s *client, server_s *server, const char *nick, response_s **response);


/**
 * Sends the specified message to the specified client.
 * @param client The client to send the message to.
 * @param message The message to be sent.
 * @return 0 on success, -1 on error
 */
int Client_Send(client_s *client, response_s *response);


void Client_HandleReadEvent(void *instance);


void Server_HandleReadEvent(void *instance);

void Client_HandleWriteEvent(void *instance);
void Server_HandleWriteEvent(void *instance);



int Client_TryEmptyBuffer(client_s *client);


int Client_HandleRead(client_s *);


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


/**
 * Try to join the client to the channel with the specified name on the given
 * server. If the channel doesn't yet exist, then it will be created, with the
 * client added as its operator.
 *
 * @param client The client to join.
 * @param server The server containing the channel to be joined to.
 * @param name The name of the channel to be joined to.
 * @return
 */
int client_join_channel(client_s *client, server_s *server, char *name);


/**
 * Sets the client's status to AWAY. While AWAY, the client will automatically send
 * the reply string to any incoming messages directed at the client (not on a channel
 * of which the client is part).
 *
 * @param client The client to set as AWAY
 * @param away_message The message to send as the automatic reply string for any incoming messages.
 */
void client_set_away(client_s *client, const char *away_message);


/**
 * Sets the client as not AWAY. This action is idempotent; if the client is already not AWAY,
 * then this action has no effect.
 *
 * @param client The client to set as not AWAY.
 */
void client_unset_away(client_s *client);

#endif //CIRC_CLIENT_H
