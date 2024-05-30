cmake_minimum_required(VERSION 3.26)
project(os_hw8 C)

set(CMAKE_C_STANDARD 11)

add_executable(os_hw8 main.c
        reader.c
        common.c)
