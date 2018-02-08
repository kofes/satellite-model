cmake_minimum_required(VERSION 3.10)
project(Satellite)

set(CMAKE_CXX_STANDARD 11)

# add mat-vec library with headers
add_subdirectory(mat-vec)
include_directories(mat-vec/inc)

# add mvp
file(GLOB MVP_SRC_FILES
        mvp/*.h mvp/*.cpp
        mvp/*/*.h mvp/*/*.cpp)
include_directories(mvp/camera)
include_directories(mvp/actions)
include_directories(mvp/projections)

# add OpenGL
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
include_directories(
        ${OPENGL_INCLUDE_DIRS}
        ${GLUT_INCLUDE_DIRS}
        ${GLEW_INCLUDE_DIRS}
)

if(NOT ${OPENGL_FOUND})
    message("OpenGL not found")
endif()

# add shader helpers
file(GLOB SHADER_SRC_FILES shader/*.h shader/*.cpp)
include_directories(shader)

# add geometric objects
file(GLOB GEOMETRIC_OBJ_SRC_FILES
        geometric_objects/*.h geometric_objects/*.cpp
        geometric_objects/*/*.h geometric_objects/*/*.cpp)
include_directories(geometric_objects)

# add SOIL
add_subdirectory(soil)
include_directories(soil/inc)

# add texture helper
set(TEXTURE_SRC_FILES texture/texture.h texture/texture.cpp)
include_directories(texture)

# set helpers
set(HELPERS args_serializer.h)

# compose executable
add_executable(Satellite
        ${MVP_SRC_FILES}
        ${SHADER_SRC_FILES}
        ${GEOMETRIC_OBJ_SRC_FILES}
        ${TEXTURE_SRC_FILES}
        ${HELPERS}
        main.cpp)

target_link_libraries(Satellite
        ${OPENGL_LIBRARIES}
        ${GLUT_LIBRARY}
        ${GLEW_LIBRARIES}
        soil
        linear_algebra)