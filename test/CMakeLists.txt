cmake_minimum_required(VERSION 3.5.1)

set(TEST_NAME ZipIterTests)

project (${TEST_NAME})




find_package(Threads REQUIRED)

include(ExternalProject)

ExternalProject_Add(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    PREFIX "${CMAKE_CURRENT_BINARY_DIR}"
    INSTALL_COMMAND ""
    )

ExternalProject_Get_Property(googletest source_dir)
set(GTEST_INCLUDE_DIRS ${source_dir}/googletest/include)



set(CXXFLAGS "-std=c++14 -O3 -Wall -Wno-deprecated -pthread")

enable_testing()


file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/*.cpp)

get_filename_component(PARENT_DIR ${PROJECT_SOURCE_DIR} DIRECTORY)

include_directories(${GTEST_INCLUDE_DIRS} ${PARENT_DIR}/include)



add_executable(${TEST_NAME} ${SRC_FILES})

target_link_libraries(${TEST_NAME} gtest gtest_main ${CMAKE_THREAD_LIBS_INIT})

add_test(test1 ${TEST_NAME})