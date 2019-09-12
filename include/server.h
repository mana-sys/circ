#ifndef SERVER_H
#define SERVER_H

#include <glib.h>

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
 * Creates a channel with the specified name and with the specified client as its operator.
 * Adds the newly created channel to the hash map of channels.
 * @param name The name of the newly created channel. This will be converted to lowercase, so it should not
 * point to a string literal.
 * @param operator The client who will become operator of the new channel.
 * @return 0 on success, -1 on failure
 */
int server_create_channel(char *name, struct client_s *operator);

#endif
