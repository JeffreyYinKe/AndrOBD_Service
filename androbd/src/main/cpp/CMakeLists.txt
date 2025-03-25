# Sets the minimum CMake version required for this project.
cmake_minimum_required(VERSION 3.22.1)

project("virtual_gamepad")

add_library(${CMAKE_PROJECT_NAME} SHARED
        src/virtual_gamepad.c src/data.c
)

# Add include directory for the executable
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(${CMAKE_PROJECT_NAME}
    android
    log)
