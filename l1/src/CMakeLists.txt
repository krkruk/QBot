cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(level_one)
add_library(level_one_impl
    "serialport.h"
    "serialportinfo.cpp"
    "serialportinfo.h"
    "messages/wheelsendmessage.cpp"
    "messages/wheelsendmessage.h"
    "messages/wheelrecvmessage.cpp"
    "messages/wheelrecvmessage.h"
    "utils/dispatcher.h"
    )

include_directories(
    ./messages
    )
add_executable(${PROJECT_NAME} "main.cpp")
target_link_libraries(${PROJECT_NAME} level_one_impl)

find_package(Boost 1.69 REQUIRED filesystem system regex thread log)
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
add_definitions(-DBOOST_LOG_DYN_LINK)
