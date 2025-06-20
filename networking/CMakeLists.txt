add_library(networking src/message.cpp src/connection.cpp)
target_compile_features(networking PUBLIC cxx_std_23)
target_include_directories(networking PUBLIC include)
target_link_libraries(networking PUBLIC asio)

add_subdirectory(test)
