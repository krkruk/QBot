cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(level_one)
add_library(lvl_one
    "serialport.cpp"
    "serialport.h"
    "serialportinfo.cpp"
    "serialportinfo.h")

add_executable(${PROJECT_NAME} "main.cpp")
target_link_libraries(${PROJECT_NAME} lvl_one)

find_package(Boost 1.69 REQUIRED filesystem system regex thread log)
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
add_definitions(-DBOOST_LOG_DYN_LINK)
