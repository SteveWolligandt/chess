add_executable(networking.test main.cpp)
target_compile_features(networking.test PUBLIC cxx_std_23)
target_link_libraries(networking.test PRIVATE networking Catch2::Catch2WithMain)

add_custom_target(
  networking.test.run
  "${CMAKE_CURRENT_BINARY_DIR}/networking.test" 
  DEPENDS networking.test
)

include(CTest)
# include(ParseAndAddCatchTests)
# include(ext/catch2/extras/ParseAndAddCatchTests)
# include(../../ext/catch2/extras/ParseAndAddCatchTests)
# ParseAndAddCatchTests(tests)
