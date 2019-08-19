#include <arpa/inet.h>
#include <glib.h>
#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include "common.h"
#include "connection.h"
#include "context.h"
#include "limits.h"
#include "log.h"
#include "server.h"

#define BACKLOG 5
#define MAXEVENTS 64

GHashTable *g_nicknames;    /* Map from nicknames to client IDs */
GHashTable *g_clients;      /* Map from client IDs to client info */

pthread_mutex_t g_nicknames_mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t g_clients_mtx   = PTHREAD_MUTEX_INITIALIZER;

static int id = 0;

struct conn_args_s {
    int fd, id;
    socklen_t len;
    struct sockaddr_in addr;
};

static void *on_accept(struct conn_args_s *arg)
{
    struct circ_client client;

    memset(&client, 0, sizeof(struct circ_client));
    client.fd = arg->fd;
    client.clientId = arg->id;

    pthread_mutex_lock(&g_clients_mtx);
    g_hash_table_insert(g_clients, GINT_TO_POINTER(client.clientId), &client);
    pthread_mutex_unlock(&g_clients_mtx);

    return NULL;
}

void client_destroy_func(gpointer data)
{
    free(data);
}

void start_server(const struct config_s conf[static 1])
{
    int sfd, cfd, epfd, ready, j;
    socklen_t len;
    char host[0xFF];
    struct sockaddr_in addr, claddr;
    struct epoll_event event;
    struct epoll_event events[MAXEVENTS];
    conn_s *conn;


    pthread_t t;

    struct conn_args_s *args;

    /* Initialize global data structures */
    g_nicknames = g_hash_table_new((GHashFunc) g_string_hash, (GEqualFunc) g_string_equal);
    g_clients   = g_hash_table_new(g_direct_hash, g_direct_equal);

    circlog(L_DEBUG, "Using hostname %s and port %u.", conf->host, conf->port);

    circlog(L_TRACE, "Creating server socket.");
    if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        logExitErr("Fatal error: socket()");
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

    event.events = EPOLLIN | EPOLLERR;
    event.data.fd = sfd;

    if (epoll_ctl(epfd, EPOLL_CTL_ADD, sfd, &event) == -1)
        logExitErr("Fatal error: epoll_ctl()");

    for (;;) {
        ready = epoll_wait(epfd, events, MAXEVENTS, -1); /* No timeout on epoll. */

        /*
         * Loop through all available events. If data can be read from the
         * server file descriptor, then we have a new connection. Additionally,
         * handle data from all current connections.
         */
        for (j = 0; j < ready; j++) {
            if (events[ready].data.fd == sfd) {
                len = sizeof(struct sockaddr_in);
                cfd = accept(events[ready].data.fd, (struct sockaddr *) &claddr, &len);
                if (cfd == -1)
                    logExitErr("Fatal error: accept()");

                /*
                 * Allocate new connection struct to represent the new
                 * connection.
                 */
                conn = calloc(1, sizeof(conn_s));
                conn->client.fd = cfd;

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
            }
        }
    }

//    for (;;) {
//        args = calloc(1, sizeof(struct conn_args_s));
//        len = sizeof(struct sockaddr_in);
//
//        cfd = accept(sfd, (struct sockaddr *) &args->addr, &args->len);
//        if (cfd == -1) {
//            circlog(L_ERROR, "Unable to accept connection.");
//            goto err;
//        }
//
//        if (getnameinfo((struct sockaddr *) &args->addr, len, host, sizeof(host), NULL, 0, 0) == -1) {
//            circlog(L_ERROR, "Unable to get name info.");
//            goto err;
//        }
//
//        if (pthread_create(&t, NULL, on_accept, NULL) != 0) {
//            circlog(L_ERROR, "Unable to create new thread for incoming connection.");
//            goto err;
//        }
//
//        pthread_detach(t);
//        id++;
//        continue;
//
//err:
//        free(args);
//
//    }

}
