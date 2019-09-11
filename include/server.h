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

#endif
