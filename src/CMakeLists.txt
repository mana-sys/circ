#cmake_minimum_required(VERSION 3.5.1)
#project(circ C)
#
#set(CMAKE_C_STANDARD 99)
set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)

add_executable(circ_sv msgtok.c circ.c circ_sv.c hashtable.c circlist.c read_message.c)
add_executable(circ_cl circ_cl.c)
target_link_libraries(circ_sv Threads::Threads)