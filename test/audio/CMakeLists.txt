add_executable(tst_audioDecoder tst_audioDecoder.cpp)
target_link_libraries(tst_audioDecoder PRIVATE audio CONAN_PKG::Catch2 CONAN_PKG::ffmpeg)
ParseAndAddCatchTests(tst_audioDecoder)
