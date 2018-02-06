cmake_minimum_required(VERSION 3.10)
project(Satellite)

set(CMAKE_CXX_STANDARD 11)

# add mat-vec library with headers
add_subdirectory(mat-vec)
include_directories(mat-vec/inc)

# add mvp
file(GLOB MVP_SRC_FILES mvp/*.cpp)
include_directories(mvp/camera)
include_directories(mvp/actions)
include_directories(mvp/projections)

# set helpers
set(HELPERS args_serializer.h)

# compose executable
add_executable(Satellite
        ${MVP_SRC_FILES}
        ${HELPERS}
        main.cpp)

target_link_libraries(Satellite linear_algebra)