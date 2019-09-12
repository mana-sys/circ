/*
 * File name:                   reactor.c
 * Date of creation:            10 September 2019
 * Date of last modification:   10 September 2019
 * Author:                      mana-
 * Description:                 Implementation of the reactor pattern.
 */

#include <stdlib.h>
#include <sys/epoll.h>

#include "log.h"
#include "reactor.h"

#define REACTOR_MAX_EVENTS 64

static int epfd;
static struct epoll_event events[REACTOR_MAX_EVENTS];


static void reactor_configure_events(struct epoll_event *event, reactor_event_handler_s *handler);


void reactor_init()
{
    if ((epfd = epoll_create1(0)) == -1)
        logExitErr("Fatal error: epoll_create1()");
}


int reactor_register_handler(reactor_event_handler_s *handler)
{
    reactor_event_handler_s *copy;
    struct epoll_event event;

    /*
     * Create a copy of the handler to be added to the list of events epoll will track.
     */
    copy = calloc(1, sizeof(reactor_event_handler_s));

    copy->fd = handler->fd;
    copy->instance = handler->instance;
    copy->handle_read = handler->handle_read;
    copy->handle_write = handler->handle_write;

    memset(&event, 0, sizeof(struct epoll_event));

    /*
     * Configure epoll event struct.
     */
//    if (copy->handle_read) {
//        event.events = EPOLLIN | EPOLLERR;
//    }
    reactor_configure_events(&event, copy);
//    event.events = EPOLLIN | EPOLLERR;
    event.data.ptr = copy;

    Log_Debug("Adding event for fd =%d", copy->fd);

    /*
     * Add to the epoll event list.
     */
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, copy->fd, &event) == -1) {
        Log_Debug("Failed to add event");
        free(copy);
        return -1;
    }

    return 0;
}


int reactor_change_registration(reactor_event_handler_s *handler)
{
    struct epoll_event event;

    memset(&event, 0, sizeof(struct epoll_event));

    /*
     * Configure epoll event struct.
     */
    reactor_configure_events(&event, handler);
    event.data.ptr = handler;

    return epoll_ctl(epfd, EPOLL_CTL_MOD, handler->fd, &event);
}


int Reactor_UnregisterHandler(reactor_event_handler_s *handler)
{
    if (epoll_ctl(epfd, EPOLL_CTL_DEL, handler->fd, NULL) == -1) {
        free(handler);
        return -1;
    }

    free(handler);
    return 0;
}


void reactor_handle_events()
{
    int ready, j;
    reactor_event_handler_s *handler;

    Log_Debug("Handling events...");


    ready = epoll_wait(epfd, events, REACTOR_MAX_EVENTS, -1); /* No timeout */


    for (j = 0; j < ready; j++) {

        handler = events[j].data.ptr;

        Log_Debug("Handling event from fd =%d", handler->fd);

        /*
         * Handle read events.
         */
        if (events[j].events & (EPOLLIN | EPOLLERR)) {
            handler->handle_read(handler->instance);
        }

        /*
         * Handle write events.
         */
        if (events[j].events & EPOLLOUT)
            handler->handle_write(handler->instance);
    }
}


static void reactor_configure_events(struct epoll_event *event, reactor_event_handler_s *handler)
{
    /*
     * Configure read events.
     */
    if (handler->handle_read) {
        event->events = EPOLLIN | EPOLLERR;
    }

    /*
     * Configure write events.
     */
    if (handler->handle_write)
        event->events = EPOLLOUT;
}