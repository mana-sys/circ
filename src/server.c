#include <arpa/inet.h>
#include <glib.h>
#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <motd.h>

#include "common.h"
#include "connection.h"
#include "limits.h"
#include "log.h"
#include "server.h"

#define BACKLOG 5
#define MAXEVENTS 64

GHashTable *g_nicknames;    /* Map from nicknames to client IDs */
GHashTable *g_clients;      /* Map from client IDs to client info */

//pthread_mutex_t g_nicknames_mtx = PTHREAD_MUTEX_INITIALIZER;
//pthread_mutex_t g_clients_mtx   = PTHREAD_MUTEX_INITIALIZER;

/*
 * Used to assign IDs to clients. This variable starts at 1 because
 * the hash table holding clients to nicknames cannot distinguish
 * between a key that does not exist and a key whose corresponding
 * value is 0.
 */
static int id = 1;


static void nicknames_hash_key_destroy_func(gpointer data)
{
    g_free(data);
}

void start_server(const struct config_s conf[static 1])
{
    char *motd;
    int sfd, cfd, epfd, ready, readyFd, j, readResult, optval;
    socklen_t len;
    struct sockaddr_in addr, claddr;
    struct epoll_event event;
    struct epoll_event events[MAXEVENTS];
    conn_s server_conn;                     /* Dummy conn_s struct for the server to be passed to epoll. */
    conn_s *conn;
    char hostname[IRC_HOSTNAME_MAX + 1];

    /* Get message of the day */
    motd = Motd_Get("motd.txt");

    /* Initialize global data structures */
    g_nicknames = g_hash_table_new_full((GHashFunc) g_str_hash, (GEqualFunc) g_str_equal,
            nicknames_hash_key_destroy_func, NULL);
    g_clients   = g_hash_table_new(g_direct_hash, g_direct_equal);

    gethostname(hostname, IRC_HOSTNAME_MAX);
    circlog(L_DEBUG, "Hostname is %s.", hostname);

    circlog(L_DEBUG, "Using hostname %s and port %u.", conf->host, conf->port);

    circlog(L_TRACE, "Creating server socket.");
    if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        logExitErr("Fatal error: socket()");

    optval = 1;
    if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
        logExitErr("Fatal error: setsockopt()");

    circlog(L_TRACE, "Server socket created successfully.");

    circlog(L_TRACE, "Binding socket...");

    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(conf->port);
    if (inet_pton(AF_INET, "0.0.0.0", &addr.sin_addr) == -1) {
        logExitErr("Fatal error: inet_pton()");
    }

    if (bind(sfd, (struct sockaddr *) &addr, sizeof(struct sockaddr_in)) == -1) {
        logExitErr("Fatal error: bind()");
    }

    circlog(L_TRACE, "Socket bound successfully.");

    if (listen(sfd, BACKLOG) == -1)
        logExitErr("Fatal error: listen()");

    circlog(L_DEBUG, "Listening on %s:%u.", conf->host, conf->port);

    /*
     * Configure epoll instance to initially include the server's file
     * descriptor.
     */
    epfd = epoll_create(64); /* Size parameter is ignored. */
    if (epfd == -1)
        logExitErr("Fatal error: epoll_create()");

    server_conn.client.fd = sfd;

    event.events = EPOLLIN | EPOLLERR;
    event.data.ptr = &server_conn;


    if (epoll_ctl(epfd, EPOLL_CTL_ADD, sfd, &event) == -1)
        logExitErr("Fatal error: epoll_ctl()");

    for (;;) {
        ready = epoll_wait(epfd, events, MAXEVENTS, -1); /* No timeout on epoll. */
        circlog(L_TRACE, "Handling %d ready file descriptor(s).", ready);
        /*
         * Loop through all available events. If data can be read from the
         * server file descriptor, then we have a new connection. Additionally,
         * handle data from all current connections.
         */
        for (j = 0; j < ready; j++) {
            readyFd = ((conn_s *) events[j].data.ptr)->client.fd;
            if (readyFd == sfd) {
                len = sizeof(struct sockaddr_in);
                cfd = accept(readyFd, (struct sockaddr *) &claddr, &len);
                if (cfd == -1)
                    logExitErr("Fatal error: accept()");

                circlog(L_INFO, "Accepting new connection.");

                /*
                 * Allocate new connection struct to represent the new
                 * connection.
                 */
                conn = calloc(1, sizeof(conn_s));
                conn->client.fd = cfd;
                conn->client.clientId = id++;
                getnameinfo((struct sockaddr *) &claddr, len, conn->client.hostname, IRC_HOSTNAME_MAX, NULL,
                        0, 0);

                conn->responses = g_queue_new();
                conn->server.clients = g_clients;
                conn->server.nicks = g_nicknames;
                conn->server.hostname = hostname;
                conn->server.motd = motd;

                /*
                 * Add client to the client hash, using the ID counter.
                 * Locking with the mutex is not needed since we are single-
                 * threaded for now.
                 */

                /* Mutex is not needed since we are single-threaded for now. */

                /* pthread_mutex_lock(&g_clients_mtx); */
                g_hash_table_insert(g_clients, GINT_TO_POINTER(conn->client.clientId), &conn->client);
                /* pthread_mutex_unlock(&g_clients_mtx); */

                /*
                 * Configure the epoll event. The event will store a pointer
                 * to the connection data so that it is easily accessible
                 * later.
                 */
                event.events = EPOLLIN | EPOLLERR;
                event.data.ptr = conn;

                /*
                 * Add the event.
                 */
                if (epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &event)) {
                    logExitErr("epoll_ctl(): fd =%d", cfd);
                }

                continue;
            } else {

                /*
                 * Ready file descriptor was a client. Read any available data
                 * and handle the resultant messages.
                 */
                circlog(L_DEBUG, "Handling connection for client =%d", ((conn_s *) events[j].data.ptr)->client.clientId);
                readResult = handle_read((conn_s *) events[j].data.ptr);

                if (readResult == CONN_RESULT_ERROR) {
                    circlog(L_WARNING, "Connection encountered error.");
                } else if (readResult == CONN_RESULT_CLOSE) {
                    circlog(L_INFO, "Client closed connection.");
//                    g_hash_table_remove(g_clients, )
                }
            }
        }
    }
}
