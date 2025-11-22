cmake_minimum_required(VERSION 3.31)
project(COMP3821GroupProject25T3)

set(CMAKE_CXX_STANDARD 14)

include_directories(./)

add_executable(COMP3821GroupProject25T3
        ./load_coordinates.cpp
        ./load_coordinates.h
        ./main.cpp)
