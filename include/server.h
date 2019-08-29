#ifndef SERVER_H
#define SERVER_H

#include "config.h"

struct context_client **clients;

/**
 * Starts the server.
 *
 * @param conf A pointer to the config_s struct to configure the server with.
 */
void start_server(const struct config_s conf[static 1]);

#endif
