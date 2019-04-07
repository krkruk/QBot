cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(level_one)
add_library(level_one_impl
    "serial/serialport.h"
    "serial/serialportinfo.cpp"
    "serial/serialportinfo.h"
    "serial/serialdevicefinder.h"
    "serial/serialdevicefinder.cpp"
    "messages/wheelsendmessage.cpp"
    "messages/wheelsendmessage.h"
    "messages/wheelrecvmessage.cpp"
    "messages/wheelrecvmessage.h"
    "model/wheel.h"
    "model/chassis.h"
    "utils/dispatcher.h"
    "utils/jsonsink.h"
    "utils/jsonsink.cpp"
    "utils/jsondispatcherprocessor.h"
    "utils/jsondispatcherprocessor.cpp"
    "commands/command.h"
    "commands/drivecommand.h"
    "commands/drivecommand.cpp"
    "commands/pwmdrivecommand.h"
    "commands/pwmdrivecommand.cpp"
    "commands/sequentialcommandexecutor.h"
    "commands/sequentialcommandexecutor.cpp"
    "rpc/chassisserviceimpl.h"
    "rpc/chassisserviceimpl.cpp"
    "rpc/grpcchassisvisitor.h"
    "rpc/grpcchassiscontroller.h"
    "rpc/grpcchassiscontroller.cpp"
)

include_directories(
    ./messages
    ./model
    ./utils
    ./commands
    ./rpc
    ./serial
    ../protos
)

add_executable(${PROJECT_NAME}
    "main.cpp"
    "grpcserver.h"
    "grpcserver.cpp"
)

target_link_libraries(${PROJECT_NAME} level_one_impl)
target_link_libraries(${PROJECT_NAME} protobuf_svc)

find_package(Boost 1.69 REQUIRED filesystem system regex thread log)
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
add_definitions(-DBOOST_LOG_DYN_LINK)

find_package(Protobuf REQUIRED)
target_link_libraries(level_one ${Protobuf_LIBRARIES})
include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
