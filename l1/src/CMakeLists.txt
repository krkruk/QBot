cmake_minimum_required(VERSION 2.8)

project(level_one)
add_executable(${PROJECT_NAME}
    "main.cpp"
    "serialport.cpp"
    "serialport.h"
    "serialportinfo.cpp"
    "serialportinfo.h")

find_package(Boost 1.69 REQUIRED filesystem system regex thread log)
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
add_definitions(-DBOOST_LOG_DYN_LINK)
