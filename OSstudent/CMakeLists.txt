cmake_minimum_required(VERSION 3.8)

project(file_student C)

set(CMAKE_C_STANDARD 23)

add_executable(file_student src/file_student.c
        src/login.c
        src/login.h
        include/macros.h
        include/utils.h
        src/command_handler.c
        src/command_handler.h)
target_include_directories(file_student PUBLIC include/ src/)


