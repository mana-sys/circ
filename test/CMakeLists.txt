set(UNITY_SOURCES vendor/unity.c)

add_executable(test_msgtok test_msgtok.c ${UNITY_SOURCES} ../src/msgtok.c)
add_executable(test_parser test_parser.c ${UNITY_SOURCES} ../src/circ.c  ../src/msgtok.c)
add_executable(test_read_message test_read_message.c ${UNITY_SOURCES} ../src/read_message.c)
add_executable(test_hashtable test_hashtable.c ${UNITY_SOURCES} ../src/hashtable.c)
add_executable(test_handlers test_handlers.c ${UNITY_SOURCES} ../src/handlers.c ../src/log.c)

target_link_libraries(test_read_message -Wl,-wrap=read)
target_link_libraries(test_hashtable m)