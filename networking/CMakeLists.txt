add_library(networking INTERFACE)
target_compile_features(networking INTERFACE cxx_std_23)
target_include_directories(networking INTERFACE include)
target_link_libraries(networking INTERFACE asio)

add_subdirectory(test)
