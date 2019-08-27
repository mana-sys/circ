set(THREADS_PREFER_PTHREAD_FLAG ON)

set(CIRC_SRC
        circ.c
        client.c
        config.c
        connection.c
        handlers.c
        hashtable.c
        log.c
        msgtok.c
        parser.c
        read_message.c
        replies.c
        server.c
        )

find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_check_modules(GLib REQUIRED glib-2.0)

include_directories(${GLib_INCLUDE_DIRS})

add_executable(circ_sv ${CIRC_SRC})
add_executable(circ_cl circ_cl.c)
add_executable(circ ${CIRC_SRC})
add_executable(ghashtable_test ghashtable_test.c)
add_definitions (${GLib_CFLAGS})

target_link_libraries(circ ${GLib_LIBRARIES} Threads::Threads m)
target_link_libraries (ghashtable_test ${GLib_LIBRARIES})

target_link_libraries(circ_sv ${GLib_LIBRARIES} Threads::Threads m)