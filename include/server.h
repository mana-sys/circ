/**
 * @file server.h
 * @brief Test file
 * Methods to interact with the circd server.
 */

#ifndef SERVER_H
#define SERVER_H

#include <glib.h>

#include "channel.h"
#include "config.h"

struct client_s;

/*
 * Structure to hold server data.
 */
typedef struct server_s {
    int             fd;         /* The server's socket */
    int             idCounter;
    GHashTable *    nicks;      /* Map from nicks (string) to client IDs (int) */
    GHashTable *    clients;    /* Map from client IDs (int) to clients (client_s *) */
    GHashTable *    channels;   /* Map from channel names (lowercase) to channel pointers. */
    GHashTable *    operators;  /* Map from operator usernames to passwords. Used for OPER command. */
    char            hostname[HOSTNAME_MAX];   /* Pointer to the server's hostname. */
    const char *    motd;       /* Message of the day. */
    uint32_t        nUsers;     /* Number of currently registered users online. */
    uint32_t        nServices;
    uint32_t        nUnknown;   /* Number of unknown connections, i.e. users that have not registered. */
    uint32_t        nOperators; /* Number of operators currently online. */
    uint32_t        nChannels;
    uint32_t        nServers;
} server_s;

/**
 * Starts the server.
 *
 * @param conf A pointer to the config_s struct to configure the server with.
 */
void start_server(const struct config_s conf[static 1]);

/**
 * Gets the channel with the specified name.
 *
 * @param name The name of the channel to get. This will be converted to lowercase, so it should not
 * point to a string literal.
 * @return Pointer to the corresponding channel struct, on success. NULL if the channel does not exist.
 */
channel_s * server_get_channel(server_s *server, char *name);

/**
 * Creates a channel with the specified name and with the specified client as its operator.
 * Adds the newly created channel to the hash map of channels.
 * @param name The name of the newly created channel. This will be converted to lowercase, so it should not
 * point to a string literal.
 * @param operator The client who will become operator of the new channel.
 * @return 0 on success, -1 on failure
 */
channel_s * server_create_channel(server_s *server, char *name, struct client_s *operator);

/**
 * Removes the given channel from the server and frees it.
 *
 * @param server
 * @param channel
 */
inline void server_remove_channel(server_s *server, channel_s *channel)
{
    g_hash_table_remove(server->channels, channel->name);
    free(channel);
}

/**
 * Generates a unique client ID.
 *
 * @param server The server that will generate the ID.
 * @return The unique client ID.
 */
inline int server_generate_id(server_s *server)
{
    int id;

    id = server->idCounter;
    server->idCounter = id + 1;

    return id;
}

#endif
