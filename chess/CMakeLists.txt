add_library(chess chessboard.cpp chesspiece.cpp networkinstance.cpp)
target_compile_features(chess PUBLIC cxx_std_23)
