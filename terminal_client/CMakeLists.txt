add_executable(terminal_client src/main.cpp)
target_link_libraries(terminal_client PUBLIC chess)
target_include_directories(terminal_client PUBLIC include)
