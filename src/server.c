#include <ctype.h>
#include <glib.h>
#include <unistd.h>

#include "client.h"
#include "connection.h"
#include "log.h"
#include "motd.h"
#include "oper.h"
#include "reactor.h"
#include "server.h"
#include "socket.h"

/*
 * Default global server struct.
 */
server_s g_server;

static void string_key_destroy_func(gpointer data);

void start_server(const struct config_s conf[static 1])
{
    reactor_event_handler_s serverHandler;

    /*
     * Used to assign IDs to clients. This variable starts at 1 because
     * the hash table holding clients to nicknames cannot distinguish
     * between a key that does not exist and a key whose corresponding
     * value is 0.
     */
    g_server.idCounter = 1;

    /*
     * Initialize server data structures.
     */
    g_server.nicks = g_hash_table_new_full((GHashFunc) g_str_hash, (GEqualFunc) g_str_equal,
            string_key_destroy_func, NULL);

    g_server.clients = g_hash_table_new(g_direct_hash, g_direct_equal);

    g_server.channels = g_hash_table_new_full((GHashFunc) g_str_hash, (GEqualFunc) g_str_equal,
            string_key_destroy_func, NULL);

    /*
     * Read entries for OPER command.
     */
    g_server.operators = g_hash_table_new_full((GHashFunc) g_str_hash, (GEqualFunc) g_str_equal,
                                               string_key_destroy_func, string_key_destroy_func);

    parse_oper_passwd(conf->oper_passwd, g_server.operators);


    /*
     * Get MOTD and hostname.
     */
    g_server.motd = Motd_Get("motd.txt", NULL);
    gethostname(g_server.hostname, IRC_HOSTNAME_MAX);

    if ((g_server.fd = Socket_Listen(conf->port)) == -1)
        logExitErr("Fatal error on socket creation");

    /*
     * Configure server event handler.
     */
    serverHandler = (reactor_event_handler_s) {.instance = &g_server, .fd = g_server.fd,
                                               .handle_read = Server_HandleReadEvent,
                                               .handle_write = NULL};

    reactor_init();
    reactor_register_handler(&serverHandler);

    circlog(L_INFO, "Hostname: %s", g_server.hostname);
    circlog(L_INFO, "Message of the day:");
    circlog(L_INFO, "\t%s", g_server.motd);
    circlog(L_DEBUG, "Listening on %s:%u", conf->host, conf->port);

    for (;;) {
        reactor_handle_events();
    }
}


int server_generate_id(server_s *server);


channel_s * server_get_channel(server_s *server, char *name)
{
    char *c;

    /*
     * Convert supplied name to lowercase.
     * TODO: Move this out to the caller.
     */
    for (c = name; *c; c++)
    {
        *c = (char) tolower((int) *c);
    }

    /*
     * Lookup returns pointer to the server struct, or NULL if not found.
     */
    return g_hash_table_lookup(server->channels, name);
}


channel_s * server_create_channel(server_s *server, char *name, struct client_s *operator)
{
    char *      c;
    channel_s * channel;

    /*
     * Convert supplied name to lowercase.
     */
    for (c = name; *c; c++)
    {
        *c = (char) tolower((int) *c);
    }

    /*
     * Create a new channel and add it to the server's channels.
     */
    channel = channel_new(name, operator);
    g_hash_table_insert(server->channels, g_strdup(name), channel);


    return channel;
}

/* NOLINTNEXTLINE */
void server_remove_channel(server_s *server, channel_s *channel);

static void string_key_destroy_func(gpointer data)
{
    g_free(data);
}
