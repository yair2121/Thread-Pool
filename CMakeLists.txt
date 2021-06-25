cmake_minimum_required(VERSION 3.16)
project(os_ex4 C)

set(CMAKE_C_STANDARD 11)

add_executable(a.out
        osqueue.c
        threadPool.c
        test_simple.c)

TARGET_LINK_LIBRARIES(a.out pthread)