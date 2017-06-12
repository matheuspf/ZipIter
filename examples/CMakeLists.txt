cmake_minimum_required(VERSION 3.5.1)
project (examples)

get_filename_component(PARENT_DIR ${PROJECT_SOURCE_DIR} DIRECTORY)

include_directories(${PARENT_DIR}/include)

set(CXXFLAGS "-std=c++14 -O3")

add_executable(LoopingExample LoopingExample.cpp)

add_executable(STLExample STLExample.cpp)