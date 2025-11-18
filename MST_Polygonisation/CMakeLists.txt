cmake_minimum_required(VERSION 3.31)
project(MST_Polygonisation)

set(CMAKE_CXX_STANDARD 20)

# Automatically find source files
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h")

# Create executable with SOURCES only (not headers or directories)
add_executable(MST_Polygonisation ${SOURCES})

# Header include path
target_include_directories(MST_Polygonisation PRIVATE include)

# -----------------------------
#  Compiler warnings (always enabled)
# -----------------------------
target_compile_options(MST_Polygonisation PRIVATE -Wall -Wextra -Wpedantic)

# -----------------------------
#  Build-type behaviour
# -----------------------------
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(MST_Polygonisation PRIVATE -g -Og)
    
    # Linux-only sanitizer support
    if (UNIX AND NOT APPLE)
        message(STATUS "✔ AddressSanitizer enabled")
        target_compile_options(MST_Polygonisation PRIVATE -fsanitize=address)
        target_link_options(MST_Polygonisation PRIVATE -fsanitize=address)
    else()
        message(WARNING "⚠ AddressSanitizer disabled (macOS does not support linking -lasan).")
    endif()

elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Optimized build: -O3 enabled.")
    target_compile_options(MST_Polygonisation PRIVATE -O3)
endif()