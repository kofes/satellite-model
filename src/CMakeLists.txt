cmake_minimum_required(VERSION 3.10)
project(Satellite)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

# add mat-vec library with headers
add_subdirectory(mat-vec)
include_directories(mat-vec)

# add mvp
file(GLOB MVP_SRC_FILES
        mvp/*.h mvp/*.cpp
        mvp/*/*.h mvp/*/*.cpp)

# add OpenGL
set(OpenGL_GL_PREFERENCE "LEGACY")
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

# add SOIL
add_subdirectory(soil)
include_directories(soil/inc)

# add texture helper
set(TEXTURE_SRC_FILES texture/texture.h texture/texture.cpp)

# set helpers
file(GLOB HELPERS_SRC_FILES helpers/*.cpp helpers/*.h)

# add object helpers
file(GLOB OBJECTS_SRC_FILES objects/*.h objects/*.cpp)

# add shapes objects
file(GLOB SHAPES_SRC_FILES
        shapes/*.h shapes/*.cpp
        shapes/*/*.h shapes/*/*.cpp)

# add math models
file(GLOB MATH_MODELS_SRC_FILES math_model/*.h math_model/*.cpp)

# include root
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(Satellite SHARED 
        ${MVP_SRC_FILES}
        ${SHADER_SRC_FILES}
        ${TEXTURE_SRC_FILES}

        ${HELPERS_SRC_FILES}

        ${OBJECTS_SRC_FILES}
        ${SHAPES_SRC_FILES}

        ${MATH_MODELS_SRC_FILES})

target_include_directories(Satellite PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Satellite
        ${OPENGL_LIBRARIES}
        ${GLUT_LIBRARY}
        ${GLEW_LIBRARIES}
        soil
        linear_algebra)

# add_subdirectory(test)

# enable_testing ()
# add_test (NAME test COMMAND TEST)