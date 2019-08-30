add_library(audio
    Audio.h
    Audio.cpp)

target_link_libraries(audio PRIVATE CONAN_PKG::ffmpeg)
