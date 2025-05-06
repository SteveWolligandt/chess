add_library(networking message.cpp connection.cpp)
target_link_libraries(networking PUBLIC asio)

add_directory(test)
