cmake_minimum_required(VERSION 3.26)

project(Chess)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_subdirectory(ext)
add_subdirectory(chess)
add_subdirectory(networking)
add_subdirectory(server)
add_subdirectory(terminal_client)

