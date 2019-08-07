project(fps_project)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(fps_project
    main.cpp)

set_property(TARGET fps_project PROPERTY CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS -pthread)
