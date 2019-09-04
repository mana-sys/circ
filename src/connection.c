#include <errno.h>
#include <unistd.h>

#include "connection.h"
#include "handlers.h"
#include "log.h"
#include "parser.h"
#include "read_message.h"


//int Conn_HandleRead (conn_s *conn)
//{
//    ssize_t numRead;
//    irc_message_s message;
//    response_s *response;
//
//    /*
//     * Perform a non-blocking read to try to fill up the rest of the
//     * store buffer. The amount of bytes to try to read is IRC_MSG_SIZE
//     * - totalRead.
//     */
//    numRead = read(conn->client.fd, conn->store, IRC_MSG_SIZE - conn->totalRead);
//    circlog(L_TRACE, "Read %ld bytes", numRead);
//    if (numRead == -1)
//        return CONN_RESULT_ERROR;
//
//    conn->totalRead += numRead;
//
//    /*
//     * Handle all messages within the store buffer.
//     */
//    while (conn_read_message(conn)) {
//        circlog(L_DEBUG, "Received message: '%s'", conn->message);
//
//        conn->responseLen = 0;
//
//        if (parse_message(conn->message, &message) != -1) {
//            Handler_HandleMessage(&conn->client, &conn->server, &message, conn->responses);
//        }
//
//        /*
//         * If we have messages in our response buffer, send them all.
//         */
////        while (!g_queue_is_empty(conn->responses)) {
////            circlog(L_DEBUG, "Sending response message.");
////
////            response = g_queue_pop_head(conn->responses);
////            write(conn->client.fd, response->response, response->len);
////
////            free(response);
////        }
//    }
//
//    /*
//     * If we read 0 bytes, then the client closed the connection.
//     * We close our side of the connection.
//     */
//    if (numRead == 0) {
//        close(conn->client.fd);
//        return CONN_RESULT_CLOSE;
//    }
//
//    return 0;
//}


//int Conn2_HandleRead (conn2_s *conn)
//{
//    ssize_t numRead;
//    irc_message_s message;
//    response_s *response;
//
//    /*
//     * Perform a non-blocking read to try to fill up the rest of the
//     * store buffer. The amount of bytes to try to read is IRC_MSG_SIZE
//     * - totalRead.
//     */
//    numRead = read(conn->fd, conn->store, IRC_MSG_SIZE - conn->totalRead);
//    circlog(L_TRACE, "Read %ld bytes", numRead);
//    if (numRead == -1)
//        return CONN_RESULT_ERROR;
//
//    conn->totalRead += numRead;
//
//    /*
//     * Handle all messages within the store buffer.
//     */
//    while (conn2_read_message(conn)) {
//        circlog(L_DEBUG, "Received message: '%s'", conn->message);
//
//        if (parse_message(conn->message, &message) != -1) {
//            Handler_HandleMessage(&conn->client, &conn->server, &message, conn->responses);
//        }
//
//        /*
//         * If we have messages in our response buffer, send them all.
//         */
////        while (!g_queue_is_empty(conn->responses)) {
////            circlog(L_DEBUG, "Sending response message.");
////
////            response = g_queue_pop_head(conn->responses);
////            write(conn->client.fd, response->response, response->len);
////
////            free(response);
////        }
//    }
//
//    /*
//     * If we read 0 bytes, then the client closed the connection.
//     * We close our side of the connection.
//     */
//    if (numRead == 0) {
//        close(conn->fd);
//        return CONN_RESULT_CLOSE;
//    }
//
//    return 0;
//}


//int Conn_HandleWrite(conn_s * conn)
//{
//    response_s *response;
//
//    while (!g_queue_is_empty(conn->responses)) {
//        circlog(L_DEBUG, "Sending response message.");
//
//        response = g_queue_pop_head(conn->responses);
//        write(conn->client.fd, response->response, response->len);
//
//        if (write(conn->client.fd, response->response, response->len) == -1) {
//            if (errno == EAGAIN)
//                conn->eagain &= EAGAIN;
//            return -1;
//        }
//
//        conn->eagain = 0;
//
//        free(response);
//    }
//
//    return -1;
//}