cmake_minimum_required(VERSION 3.2)
project(TEST_Satellite)

set(CMAKE_CXX_STANDARD 11)

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

include_directories(${TEST_SOURCE_DIR}/src)

file(GLOB TEST_SOURCES src/*.cpp src/*.h)

add_executable(TEST_Satellite ${TEST_SOURCES} main.cpp)

target_link_libraries(TEST_Satellite Satellite ${GTEST_BOTH_LIBRARIES})
