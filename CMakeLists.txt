cmake_minimum_required(VERSION 3.20)
project(dahacks5 LANGUAGES CXX)

add_executable(main
    main/main.cpp
)
target_link_libraries(main PRIVATE user32 comctl32)
target_compile_definitions(main PRIVATE UNICODE _UNICODE)

set_target_properties(main PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build"
)
