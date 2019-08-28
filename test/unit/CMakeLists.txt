
find_package(PkgConfig REQUIRED)

pkg_check_modules(GLib REQUIRED glib-2.0)

include_directories(${GLib_INCLUDE_DIRS})

include_directories(vendor)
include_directories(${CMAKE_SOURCE_DIR}/include)

set(UNITY_SOURCES vendor/unity.c)
set(UNITY_W_FIXTURE_SOURCES vendor/unity.c vendor/unity_fixture.c)

set(COVERAGE_COMPILER_FLAGS -fprofile-arcs -ftest-coverage)
set(COVERAGE_LINKER_FLAGS --coverage)

add_compile_options(-fprofile-arcs -ftest-coverage)
add_link_options(${COVERAGE_LINKER_FLAGS})

add_executable(test_msgtok test_msgtok.c ${UNITY_SOURCES} ${CMAKE_SOURCE_DIR}/src/msgtok.c)
add_executable(test_parser test_parser.c ${UNITY_SOURCES} ${CMAKE_SOURCE_DIR}/src/msgtok.c ${CMAKE_SOURCE_DIR}/src/parser.c ${CMAKE_SOURCE_DIR}/src/log.c)
add_executable(test_read_message test_read_message.c ${UNITY_SOURCES} ${CMAKE_SOURCE_DIR}/src/read_message.c)
add_executable(test_hashtable test_hashtable.c ${UNITY_SOURCES} ${CMAKE_SOURCE_DIR}/src/hashtable.c)
add_executable(test_handlers test_handlers.c ${UNITY_SOURCES} ${CMAKE_SOURCE_DIR}/src/handlers.c ${CMAKE_SOURCE_DIR}/src/log.c ${CMAKE_SOURCE_DIR}/src/hashtable.c
        ${CMAKE_SOURCE_DIR}/src/replies.c ${CMAKE_SOURCE_DIR}/src/client.c)
add_executable(test_read_message1 test_read_message1.c ${UNITY_SOURCES} ${CMAKE_SOURCE_DIR}/src/read_message.c)
#add_executable(hashtable_test hashtable_test.c hashtable_test_runner.c ${UNITY_SOURCES})



target_compile_options(test_msgtok PUBLIC ${COVERAGE_COMPILER_FLAGS})
target_link_libraries(test_read_message -Wl,-wrap=read)


target_link_libraries(test_hashtable m)
target_link_libraries(test_handlers m ${GLib_LIBRARIES})
