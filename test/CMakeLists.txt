cmake_minimum_required(VERSION 2.8.8)

set(TEST_NAME ZipIterTests)

project (${TEST_NAME})


find_package(Threads REQUIRED)

set(CXXFLAGS "-std=c++14 -O3 -Wall -Wno-deprecated -pthread")

add_subdirectory(gtest)

enable_testing()


get_filename_component(PARENT_DIR ${PROJECT_SOURCE_DIR} DIRECTORY)

include_directories(${GTEST_INCLUDE_DIRS} ${PARENT_DIR}/include)


file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/*.cpp)

add_executable(${TEST_NAME} ${SRC_FILES})

add_dependencies(${TEST_NAME} googletest)


target_link_libraries(${TEST_NAME} ${GTEST_LIBS_DIR}/libgtest.a ${GTEST_LIBS_DIR}/libgtest_main.a)

target_link_libraries(${TEST_NAME} ${CMAKE_THREAD_LIBS_INIT})

add_test(test1 ${TEST_NAME})