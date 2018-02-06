cmake_minimum_required(VERSION 3.10)
project(Satellite)

set(CMAKE_CXX_STANDARD 11)

set(HELPERS args_serializer.h)

add_executable(Satellite ${HELPERS} main.cpp)