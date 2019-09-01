#include "client.h"
#include "connection.h"
#include "log.h"
#include "replies.h"

void Client_TryRegister (client_s *client, server_s *server, response_s **response)
{

    if (!client->registered && client->receivedUser && client->receivedNick) {

        circlog(L_INFO, "Client successfully completed registration; sending RPL_WELCOME.");

        *response = calloc(1, sizeof(response_s));

        /*
         * Insert the client into the clients hash, and send the RPL_WELCOME message.
         */
        g_hash_table_insert(server->clients, GINT_TO_POINTER(client->clientId), client);
        client->registered = true;
        (*response)->len = Reply_RplWelcome(client, (*response)->response);
        return;
    }

    /*
     * No message to be sent.
     */
    *response = NULL;
}

int Client_TryChangeNick (client_s *client, server_s *server, const char *nick, response_s **response)
{
    /*
     * ERR_NICKNAMEINUSE if nickname is already taken by another client.
     */
    if (g_hash_table_contains(server->nicks, nick)) {

        Log_Info("Nickname '%s' already in use. Sending ERR_NICKNAMEINUSE.", nick);

        *response = calloc(1, sizeof(response_s));
        (*response)->len = Reply_ErrNicknameInUse(client, nick, (*response)->response);

        return -1;

    } else {

        /*
         * If the client already has a nickname, then delete the old entry from
         * the hash and add the new one.
         */
        if (client->receivedNick) {
            Log_Info("Removing old nickname %s", client->nickname);
            g_hash_table_remove(server->nicks, client->nickname);
        }

        /*
         * Add the new nickname to the client.
         */
        Log_Info("Adding nickname =%s to for client =%d", nick, client->clientId);

        g_hash_table_insert(server->nicks, g_strdup(nick), GINT_TO_POINTER(client->clientId));
        strcpy(client->nickname, nick);
        client->receivedNick = true;
    }

    return 0;
}


int Client_Send(client_s *client, const char *message)
{

}

int Server_TrySend (server_s *server, client_s *from_client, const char *nick, const char *message, char *response, size_t *len)
{
    gpointer clientId;
    client_s *client;

    clientId = g_hash_table_lookup(server->nicks, nick);
    if (clientId == 0) {
        *len = Reply_ErrNoSuchNick(from_client, nick, response);
        return -1;
    }

    client = g_hash_table_lookup(server->clients, GINT_TO_POINTER(clientId));
    return Client_Send(client, message);
}