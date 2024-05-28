cmake_minimum_required(VERSION 3.10)
project(femtoshell)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -Wall -Werror -Wextra -pedantic -std=c2x")

set(SOURCE_FILES "main.c")

add_executable(femtoshell "main.c")
