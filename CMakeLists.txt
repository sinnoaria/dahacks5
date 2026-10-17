cmake_minimum_required(VERSION 3.20)
project(dahacks5 LANGUAGES CXX)

add_executable(main main/main.cpp)

set_target_properties(main PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build"
)
