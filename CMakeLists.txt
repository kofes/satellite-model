cmake_minimum_required(VERSION 3.10)
project(Satellite)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

# add mat-vec library with headers
add_subdirectory(src)
include_directories(src)

# include root
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# compose executable
add_executable(main main.cpp)

target_link_libraries(main
#        ${OPENGL_LIBRARIES}
#        ${GLUT_LIBRARY}
#        ${GLEW_LIBRARIES}
        # soil
        Satellite)
