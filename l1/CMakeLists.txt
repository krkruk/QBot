cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(l1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory("src")
add_subdirectory("tests")
add_subdirectory("protos")
