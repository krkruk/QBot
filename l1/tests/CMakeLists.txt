cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(core_test)
enable_testing()


add_executable(core_tests
    test_core.cpp)

add_executable(core_chassis_test
    test_chassis_model.cpp
    )


add_compile_options(-Wno-disabled-macro-expansion)
target_include_directories(
    core_tests
    PRIVATE ${BOOST_INCLUDE_DIRS}
    )
find_package(Boost 1.69 REQUIRED filesystem system regex thread log)
target_link_libraries(core_tests ${Boost_LIBRARIES})
add_definitions(-DBOOST_LOG_DYN_LINK)

include_directories(
    ../src
    ../src/messages
    ../src/utils
    )
link_directories(
    ../src
    ../src/messages
    ../src/utils
    )
target_link_libraries(core_tests level_one_impl)

add_test(
    NAME test_core
    COMMAND $<TARGET_FILE:core_tests>
)

add_test(
    NAME test_chassis_model
    COMMAND $<TARGET_FILE:core_chassis_test>
)
