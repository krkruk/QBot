cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(core_test)
enable_testing()


add_executable(core_tests
    test_core.cpp)

add_executable(core_chassis_test
    test_chassis_model.cpp
    )


add_compile_options(-Wno-disabled-macro-expansion)
find_package(Boost 1.69 REQUIRED filesystem system regex thread log)
add_definitions(-DBOOST_LOG_DYN_LINK)

include_directories(
    ../src
    ../src/serial
    ../src/messages
    ../src/model
    ../src/utils
    ../src/commands
    ../src/rpc
    ../protos
    )
link_directories(
    ../src
    ../src/serial
    ../src/messages
    ../src/model
    ../src/commands
    ../src/rpc
    ../protos
    )


target_include_directories(core_tests PRIVATE ${BOOST_INCLUDE_DIRS})
target_link_libraries(core_tests level_one_impl ${Boost_LIBRARIES})
add_test(
    NAME test_core
    COMMAND $<TARGET_FILE:core_tests>
)

find_package(Protobuf REQUIRED)
target_include_directories(core_chassis_test PRIVATE
    ${BOOST_INCLUDE_DIRS}
    ${Protobuf_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}
)
target_link_libraries(core_chassis_test
    level_one_impl
    protobuf_svc
    ${Boost_LIBRARIES}
    ${Protobuf_LIBRARIES}
)
add_test(
    NAME test_chassis_model
    COMMAND $<TARGET_FILE:core_chassis_test>
)
