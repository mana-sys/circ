#include <glib.h>
#include <unistd.h>

#include "client.h"
#include "connection.h"
#include "log.h"
#include "motd.h"
#include "reactor.h"
#include "server.h"
#include "socket.h"


/*
 * Default global server struct.
 */
server_s server;

/*
 * Used to assign IDs to clients. This variable starts at 1 because
 * the hash table holding clients to nicknames cannot distinguish
 * between a key that does not exist and a key whose corresponding
 * value is 0.
 */
static int id = 1;


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
    server.idCounter = 1;

    /*
     * Initialize server data structures.
     */
    server.nicks = g_hash_table_new_full((GHashFunc) g_str_hash, (GEqualFunc) g_str_equal,
            string_key_destroy_func, NULL);

    server.clients = g_hash_table_new(g_direct_hash, g_direct_equal);

    server.channels = g_hash_table_new_full((GHashFunc) g_str_hash, (GEqualFunc) g_str_equal,
            string_key_destroy_func, NULL);

    /*
     * Get MOTD and hostname.
     */
    server.motd = Motd_Get("motd.txt", NULL);
    gethostname(server.hostname, IRC_HOSTNAME_MAX);

    if ((server.fd = Socket_Listen(conf->port)) == -1)
        logExitErr("Fatal error on socket creation");

    /*
     * Configure server event handler.
     */
    serverHandler = (reactor_event_handler_s) {.instance = &server, .fd = server.fd,
                                               .handle_read = Server_HandleReadEvent,
                                               .handle_write = NULL};

    reactor_init();
    reactor_register_handler(&serverHandler);

    circlog(L_INFO, "Hostname: %s", server.hostname);
    circlog(L_INFO, "Message of the day:");
    circlog(L_INFO, "\t%s", server.motd);
    circlog(L_DEBUG, "Listening on %s:%u", conf->host, conf->port);

    for (;;) {
        reactor_handle_events();
    }
}


//int server_create_channel()
static void string_key_destroy_func(gpointer data)
{
    g_free(data);
}
