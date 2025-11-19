cmake_minimum_required(VERSION 3.31)
project(DT_CES_SA)

set(CMAKE_CXX_STANDARD 20)

# Automatically find src/ and include/
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h")

add_executable(DT_CES_SA ${SOURCES} ${HEADERS}
        include/FlipCriteria.h
        src/FlipCriteria.cpp
        src/GraphState.cpp)

# Header include path
target_include_directories(DT_CES_SA PRIVATE include)

# -----------------------------
#  Compiler warnings (always enabled)
# -----------------------------
target_compile_options(DT_CES_SA PRIVATE -Wall -Wextra -Wpedantic)

# -----------------------------
#  Build-type behaviour
# -----------------------------
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")

    # Linux-only sanitizer support
    if (UNIX AND NOT APPLE)
        message(STATUS "AddressSanitizer enabled")
        target_compile_options(DT_CES_SA PRIVATE -fsanitize=address -g -Og)
        target_link_options(DT_CES_SA PRIVATE -fsanitize=address)
    else()
        message(WARNING "AddressSanitizer disabled (macOS does not support linking -lasan).")
        target_compile_options(DT_CES_SA PRIVATE -g -Og)
    endif()

elseif (CMAKE_BUILD_TYPE STREQUAL "Release")

    message(STATUS "Optimized build: -O3 enabled.")
    target_compile_options(DT_CES_SA PRIVATE -O3)

endif()
