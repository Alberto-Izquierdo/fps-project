project(fps_project)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(fps_project
    main.cpp
    MainApplication.h
    MainApplication.cpp)

add_subdirectory(audio)

include_directories(${OGRE_INCLUDE_DIRS})
link_directories(${OGRE_LIBRARY_DIRS})

target_link_libraries(fps_project PRIVATE CONAN_PKG::OGRE)

set_property(TARGET fps_project PROPERTY CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS -pthread)

#if (CMAKE_BUILD_TYPE STREQUAL "Debug")
#configure_file(${CONAN_OGRE_ROOT}/share/OGRE/plugins_d.cfg
#${CMAKE_BINARY_DIR}/bin/plugins.cfg COPYONLY)
#else ()
#configure_file(${CONAN_OGRE_ROOT}/share/OGRE/plugins.cfg
#${CMAKE_BINARY_DIR}/bin/plugins.cfg COPYONLY)
#endif ()
