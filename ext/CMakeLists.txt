# add ASIO interface library with only an include directory
add_library(asio INTERFACE)
target_include_directories(asio INTERFACE asio/asio/include)

# Catch2 for testing
add_subdirectory(catch2)
