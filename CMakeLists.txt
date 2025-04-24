cmake_minimum_required(VERSION 3.26)

project(Chess)

add_subdirectory(chess)
add_subdirectory(server)
add_subdirectory(terminal_client)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
