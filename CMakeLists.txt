cmake_minimum_required(VERSION 3.14)
project(circ C)

set(CMAKE_C_STANDARD 99)

add_subdirectory(src)
add_subdirectory(test)

enable_testing()
add_test(NAME test_msgtok COMMAND test_msgtok)
add_test(NAME test_parser COMMAND test_parser)
add_test(NAME test_read_message COMMAND test_read_message)