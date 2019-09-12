#include <ctype.h>
#include <irc.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>

#include "client.h"
#include "connection.h"
#include "handlers.h"
#include "log.h"
#include "parser.h"
#include "reactor.h"
#include "read_message.h"
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
        client->server->nUnknown--;
        client->server->nUsers++;
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


int Client_Send(client_s *client, response_s *response)
{
    g_queue_push_tail(client->conn.responses, response);
    return Client_TryEmptyBuffer(client);
}

int Server_TrySend (server_s *server, client_s *from_client, const char *nick, const char *message, char *response, size_t *len)
{
//    gpointer clientId;
//    client_s *client;
//
//    clientId = g_hash_table_lookup(server->nicks, nick);
//    if (clientId == 0) {
//        *len = Reply_ErrNoSuchNick(from_client, nick, response);
//        return -1;
//    }
//
//    client = g_hash_table_lookup(server->clients, GINT_TO_POINTER(clientId));
//    return Client_Send(client, message);
}

int Client_HandleRead(client_s * client)
{
    ssize_t numRead;
    irc_message_s message;
    response_s *response;
    conn_s *conn;

    conn = &client->conn;

    /*
     * Read as many bytes as possible from the connection's file descriptor.
     */
    if ((numRead = Conn_ReadStoreBuffer(conn)) == -1)
        return -1;

    /*
     * Handle all messages within the store buffer.
     */
    while (conn_read_message(conn)) {
        circlog(L_DEBUG, "Received message: '%s'", conn->message);

        if (parse_message(conn->message, &message) != -1) {
            Handler_HandleMessage(client, client->server, &message, conn->responses);
        }

        while (!g_queue_is_empty(conn->responses)) {
            circlog(L_DEBUG, "Sending response message.");

            response = g_queue_pop_head(conn->responses);
            write(client->conn.fd, response->response, response->len);

            free(response);
        }
    }

    /*
     * If the connection was closed, close the connection's file descriptor
     * and return.
     */
    if (numRead == 0) {
//        Conn_Close(conn);
        close(conn->fd);

        if (client->registered)
            client->server->nUsers--;
        else
            client->server->nUnknown--;

        return CONN_RESULT_CLOSE;
    }

    return 0;
}


void Client_HandleReadEvent(void *instance)
{
    int result;
    client_s *client;

    client = instance;

    result = Client_HandleRead(client);

    if (result == CONN_RESULT_ERROR) {
        circlog(L_WARNING, "Connection encountered error.");
    } else if (result == CONN_RESULT_CLOSE) {
        circlog(L_INFO, "Client closed connection.");
    }
}


void Client_HandleWriteEvent(void *instance)
{

}


void Server_HandleWriteEvent(void *instance)
{

}


void Server_HandleReadEvent(void *instance)
{
    int                     flags;
    socklen_t               len;
    server_s *              server;
    client_s *              client;
    reactor_event_handler_s handler;
    struct sockaddr_in      addr;

    server = instance;
    len = sizeof(struct sockaddr_in);
    client = calloc(1, sizeof(client_s));

    /*
     * Accept the connection.
     */
    if ((client->conn.fd = accept(server->fd, (struct sockaddr *) &addr, &len)) == -1) {
        Log_Error("Unable to accept connection.");
        goto error;
    }

    /*
     * Fill the fields of the client struct.
     */
    client->clientId = server->idCounter++;
    client->conn.responses = g_queue_new();
    client->server = server;
    getnameinfo((struct sockaddr *) &addr, len, client->hostname, IRC_HOSTNAME_MAX, NULL, 0, 0);


    server->nUnknown++;


    g_hash_table_insert(server->clients, GINT_TO_POINTER(client->clientId), client);

    handler.fd = client->conn.fd;
    handler.instance = client;
    handler.handle_read = Client_HandleReadEvent;
    handler.handle_write = NULL;

    if (reactor_register_handler(&handler) == -1) {
        Log_Error("Failed to register handler for client =%d", client->clientId);
        goto error;
    }

    return;

    error:
    free(client);
}


int Client_TryEmptyBuffer(client_s *client)
{
    response_s *response;
    while (!g_queue_is_empty(client->conn.responses)) {

        response = g_queue_pop_head(client->conn.responses);
        write(client->conn.fd, response->response, response->len);

        free(response);
    }

    return 0;
}


int client_join_channel(client_s *client, server_s *server, char *name)
{

    char *c;
    channel_s *channel;

    /*
     * Lowercase the channel name.
     */
    for (c = name; *c != '\0'; c++)
    {
        *c = (char) tolower((int) *c);
    }

    /*
     * Verify the channel name. If the channel name is invalid, send ERR_NOSUCHCHANNEL.
     */
    if (channel_verify_name(name) == -1) {

        /*
         * TODO: Send ERR_NOSUCHCHANNEL
         */
        return -1;
    }

    /*
     * Check if channel with the name exists on the server.
     */
    channel = g_hash_table_lookup(server->channels, name);

    /*
     * If the channel exists, join the client to that channel.
     */
    if (channel)
        return channel_join(channel, client);

    /*
     * If the channel doesn't exist, then create the channel.
     */
    channel = channel_new(name, client);

    g_hash_table_insert(server->channels, g_strdup(name), channel);

    return 0;





    return 0;
}
