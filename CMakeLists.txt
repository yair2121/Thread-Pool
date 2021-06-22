cmake_minimum_required(VERSION 3.16)
project(EX4 C)

set(CMAKE_C_STANDARD 99)

add_executable(EX4 test.c osqueue.c threadPool.h threadPool.h.c)