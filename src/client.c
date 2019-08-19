#include "client.h"

int client_register (client_s *client, server_s *server, char *response)
{
    return (int) g_hash_table_insert(server->clients, GINT_TO_POINTER(client->clientId), client);
}