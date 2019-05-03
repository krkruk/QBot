cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(l1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(WITH_SINGLE_UC "With single microcontroller" ON)

if(WITH_SINGLE_UC)
    add_definitions(-DSINGLE_UC)
else()
    add_definitions(-DMULTIPLE_UC)
endif()

add_subdirectory("src")
add_subdirectory("tests")
add_subdirectory("protos")
