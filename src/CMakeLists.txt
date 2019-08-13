set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_check_modules(GLib REQUIRED glib-2.0)

include_directories(${GLib_INCLUDE_DIRS})

add_executable(circ_sv msgtok.c circ_sv.c hashtable.c circlist.c read_message.c log.c handlers.c parser.c)
add_executable(circ_cl circ_cl.c)
add_executable(circ circ.c config.c log.c server.c)
add_executable(ghashtable_test ghashtable_test.c)
add_definitions (${GLib_CFLAGS})

target_link_libraries(circ ${GLib_LIBRARIES})
target_link_libraries (ghashtable_test ${GLib_LIBRARIES})

target_link_libraries(circ_sv Threads::Threads m)