cmake_minimum_required(VERSION 3.10)
project(Satellite)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory(mat-vec)

include_directories(mat-vec/inc)

set(HELPERS args_serializer.h)

add_executable(Satellite ${HELPERS} main.cpp)

target_link_libraries(Satellite linear_algebra)