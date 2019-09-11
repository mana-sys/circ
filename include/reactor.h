/*
 * File name:                   reactor.h
 * Date of creation:            10 September 2019
 * Date of last modification:   10 September 2019
 * Author:                      mana-
 * Description:                 Implementation of the reactor pattern, using Linux's epoll as
 *                              the demultiplexing mechanism.
 *
 * Declarations:
 *
 *      void Reactor_Init               ();
 *      int  Reactor_RegisterHandler    (reactor_event_handler_s *handler);
 *      int  Reactor_ChangeRegistration (reactor_event_handler_s *handler);
 *      int  Reactor_UnregisterHandler  (reactor_event_handler_s *handler);
 *
 */

#ifndef CIRC_REACTOR_H
#define CIRC_REACTOR_H

typedef void  (*handle_read_event_func)(void *);
typedef void (*handle_write_event_func)(void *);


/*
 * Represents an event handler to which the reactor will dispatch events.
 */
typedef struct {
    int                     fd;
    void *                  instance;
    handle_read_event_func  handle_read;
    handle_write_event_func handle_write;
} reactor_event_handler_s;


/**
 * Initializes the reactor singleton.
 */
void Reactor_Init();


/**
 * Registers the specified handler with the given reactor. If the handle_read field on the
 * handler is not NULL, then the epoll_event structure will be configured for EPOLLIN and EPOLLERR.
 * If the handle_write field on the handler is not NULL, then the epoll_event structure will be
 * configured for EPOLLOUT.
 *
 * @param handler The handler to register.
 * @return 0 on success, -1 on failure.
 */
int Reactor_RegisterHandler(reactor_event_handler_s *handler);


/**
 * Changes the registration of the specified handler, which has already been registered. Works in
 * the same fashion as Reactor_RegisterHandler.
 *
 * @param handler The handler for which to change the registration.
 * @return 0 on success, -1 on failure.
 */
int Reactor_ChangeRegistration(reactor_event_handler_s *handler);


/**
 * Unregisters the specified handler, removing it from the epoll event list.
 *
 * @param handler The handler to unregister.
 * @return 0 on success, -1 on failure.
 */
int Reactor_UnregisterHandler(reactor_event_handler_s *handler);


/**
 * Retrieves events from epoll and dispatches them to their corresponding event handlers.
 */
void Reactor_HandleEvents();

#endif //CIRC_REACTOR_H
