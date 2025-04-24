add_library(chess chessboard.cpp chesspiece.cpp networkinstance.cpp)
target_compile_features(mytarget PUBLIC cxx_std_23)
