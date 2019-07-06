cmake_minimum_required(VERSION 3.14)
project(circ C)

set(CMAKE_C_STANDARD 99)

add_executable(circ_sv circ_sv.c)
add_executable(circ_cl circ_cl.c)