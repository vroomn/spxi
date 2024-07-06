# CMake config for the C/C++ spxi read/write library

cmake_minimum_required(VERSION 3.27.0)

project(spxi_lib
        VERSION 0.01.00
        LANGUAGES C)

set(CMAKE_C_STANDARD 23)

set(CMAKE_C_FLAGS "-Wall")

add_library(spxi_lib src/spxi_lib.c)

# ------- Library output name customization ---------------------------
# Remove the "lib" prefix off of the library name
set_target_properties(spxi_lib PROPERTIES PREFIX "")
# Set the name to simply "spxi"
set_target_properties(spxi_lib PROPERTIES OUTPUT_NAME "spxi")
# Set the file extention to ".lib", the windows equivalent of .a, makes no difference in actual running of the program
set_target_properties(spxi_lib PROPERTIES SUFFIX ".lib")

# Precompiled binary, made on windows
set_target_properties(spxi_lib PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)