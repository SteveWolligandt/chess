add_library(chess src/chessboard.cpp src/chesspiece.cpp src/networkinstance.cpp)
target_compile_features(chess PUBLIC cxx_std_23)
target_include_directories(chess PUBLIC include)
