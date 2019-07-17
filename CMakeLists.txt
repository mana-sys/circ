cmake_minimum_required(VERSION 3.5.1)
project(circ C)

set(CMAKE_C_STANDARD 99)

add_subdirectory(src)
add_subdirectory(test)

enable_testing()
add_test(NAME test_msgtok COMMAND test_msgtok)