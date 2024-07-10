# This simply exists to compose a testing file to execute the library and prove it works

cmake_minimum_required(VERSION 3.27.0)

project(spxi_testing
        LANGUAGES C)

set(CMAKE_C_STANDARD 11)

add_subdirectory(spxi_lib)

add_executable(spxi_testing tests/write_test.c)

target_link_libraries(spxi_testing PUBLIC spxi_lib)

target_include_directories(spxi_testing PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           "${PROJECT_SOURCE_DIR}/spxi_lib/src")