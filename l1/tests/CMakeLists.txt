cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(core_test)
enable_testing()

add_executable(core_tests test_core_model.cpp)
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
    )
link_directories(
    ../src
    ../src/messages
    )
target_link_libraries(core_tests level_one_impl)

add_test(
    NAME test_core_model
    COMMAND $<TARGET_FILE:core_tests>
)

