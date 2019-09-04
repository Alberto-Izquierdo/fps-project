set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
add_executable(tst_audioDecoder tst_audioDecoder.cpp)
target_link_libraries(tst_audioDecoder audio CONAN_PKG::Catch2)
ParseAndAddCatchTests(tst_audioDecoder)
