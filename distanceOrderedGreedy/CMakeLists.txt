cmake_minimum_required(VERSION 3.31)
project(distanceOrderedGreedy)

set(CMAKE_CXX_STANDARD 20)

# Automatically find src/ and include/
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h")

add_executable(distanceOrderedGreedy ${SOURCES} ${HEADERS}
        include/geometry.h
        include/hull.h
        include/triangulate.h
        include/load_coordinates.h
        src/geometry.cpp
        src/hull.cpp
        src/triangulate.cpp
        src/load_coordinates.cpp)

# Header include path
target_include_directories(distanceOrderedGreedy PRIVATE include)

# -----------------------------
#  Compiler warnings (always enabled)
# -----------------------------
target_compile_options(distanceOrderedGreedy PRIVATE -Wall -Wextra -Wpedantic)

# -----------------------------
#  Build-type behaviour
# -----------------------------
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")

    # Linux-only sanitizer support
    if (UNIX AND NOT APPLE)
        message(STATUS "AddressSanitizer enabled")
        target_compile_options(distanceOrderedGreedy PRIVATE -fsanitize=address -g -Og)
        target_link_options(distanceOrderedGreedy PRIVATE -fsanitize=address)
    else()
        message(WARNING "AddressSanitizer disabled (macOS does not support linking -lasan).")
        target_compile_options(distanceOrderedGreedy PRIVATE -g -Og)
    endif()

elseif (CMAKE_BUILD_TYPE STREQUAL "Release")

    message(STATUS "Optimized build: -O3 enabled.")
    target_compile_options(distanceOrderedGreedy PRIVATE -O3)

endif()
