add_executable(networking_test main.cpp)
target_compile_features(networking_test PUBLIC cxx_std_23)
target_link_libraries(networking_test PRIVATE networking Catch2::Catch2WithMain)

# include(CTest)
# include(ParseAndAddCatchTests)
# include(ext/catch2/extras/ParseAndAddCatchTests)
# include(../../ext/catch2/extras/ParseAndAddCatchTests)
# ParseAndAddCatchTests(tests)
