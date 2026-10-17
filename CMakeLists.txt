cmake_minimum_required(VERSION 3.20)
project(dahacks5 LANGUAGES CXX)

add_library(fitness_gui_module STATIC
    main/modules/fitnessGui.cpp
)
target_link_libraries(fitness_gui_module PUBLIC user32)

add_executable(main
    main/main.cpp
)
target_link_libraries(main PRIVATE fitness_gui_module)

set_target_properties(main PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build"
)
