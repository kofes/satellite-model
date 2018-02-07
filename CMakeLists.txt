cmake_minimum_required(VERSION 3.10)
project(Satellite)

set(CMAKE_CXX_STANDARD 11)

# add mat-vec library with headers
add_subdirectory(mat-vec)
include_directories(mat-vec/inc)

# add mvp
file(GLOB MVP_SRC_FILES mvp/*.h mvp/*.cpp)
include_directories(mvp/camera)
include_directories(mvp/actions)
include_directories(mvp/projections)

# add OpenGL
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS})

# add shader helpers
file(GLOB SHADER_SRC_FILES shader/*.h shader/*.cpp)
include_directories(shader)

# set helpers
set(HELPERS args_serializer.h)

# compose executable
add_executable(Satellite
        ${MVP_SRC_FILES}
        ${SHADER_SRC_FILES}
        ${HELPERS}
        main.cpp)

target_link_libraries(Satellite
        ${OPENGL_LIBRARIES}
        ${GLUT_LIBRARY}
        ${GLEW_LIBRARY}
        linear_algebra)