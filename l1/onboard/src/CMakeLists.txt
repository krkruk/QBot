cmake_minimum_required(VERSION 2.8)

project(level_one)
add_executable(${PROJECT_NAME}
    "main.cpp"
    "serialport.cpp"
    "serialport.h")

find_package(Boost 1.69 REQUIRED system regex thread)
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
