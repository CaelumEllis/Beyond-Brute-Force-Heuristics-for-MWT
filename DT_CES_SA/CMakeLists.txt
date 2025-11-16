cmake_minimum_required(VERSION 3.31)
project(DT_CES_SA)

set(CMAKE_CXX_STANDARD 20)

# Automatically find all .cpp files under src/ and .h under include/
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h")

add_executable(DT_CES_SA ${SOURCES} ${HEADERS}
        include/coordinate.h
        include/load_coordinates.h
        src/load_coordinates.cpp
        include/SimpleDelaunay.h
        include/DelaunayWrapper.h
        src/DelaunayWrapper.cpp
        include/GraphState.h)

# Tell compiler where to find headers
target_include_directories(DT_CES_SA PRIVATE include)

# -----------------------------
#  Compiler warnings (always enabled)
# -----------------------------
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(DT_CES_SA PRIVATE -Wall -Wextra -Wpedantic)
endif()

# -----------------------------
#  Sanitizers ONLY in Debug mode
# -----------------------------
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(DT_CES_SA PRIVATE -fsanitize=address -g -Og)
    target_link_options(DT_CES_SA PRIVATE -fsanitize=address)
endif()

# -----------------------------
#  Optimizations for Release mode
# -----------------------------
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(DT_CES_SA PRIVATE -O3)
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(DT_CES_SA PRIVATE -Wall -Wextra -Wpedantic)
    target_link_options(DT_CES_SA PRIVATE -fsanitize=address)
endif()
