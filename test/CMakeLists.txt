cmake_minimum_required(VERSION 3.2)
project(SATELLITE_TEST)

set(CMAKE_CXX_STANDARD 11)

include_directories(${SATELLITE_TEST_SOURCE_DIR}/inc)

file(GLOB SATELLITE_TEST_SOURCE_FILES ${SATELLITE_TEST_SOURCE_DIR}/src/*.cpp)

add_executable(SATELLITE_TEST ${SATELLITE_TEST_SOURCE_FILES} ${SATELLITE_TEST_SOURCE_DIR}/main.cpp)

target_link_libraries(SATELLITE_TEST linear_algebra)