cmake_minimum_required(VERSION 3.14)
project(circ C)

set(CMAKE_C_STANDARD 99)
find_package(PythonInterp REQUIRED)

include_directories(include)

add_subdirectory(src)
add_subdirectory(test)

enable_testing()
add_test(NAME unit_test_msgtok COMMAND test_msgtok)
add_test(NAME unit_test_parser COMMAND test_parser)
add_test(NAME unit_test_read_message COMMAND test_read_message)
add_test(NAME unit_test_hashtable COMMAND test_hashtable)
add_test(NAME unit_test_handlers COMMAND test_handlers)
add_test(NAME unit_test_read_message1 COMMAND test_read_message1)
add_test(NAME unit_test_motd COMMAND test_motd)
add_test(NAME unit_test_channel COMMAND test_channel)
add_test(NAME unit_test_vector COMMAND test_vector)

add_test(NAME integration_test_register
        COMMAND pytest --circ $<TARGET_FILE:circ>
        ${CMAKE_CURRENT_SOURCE_DIR}/test/integration)