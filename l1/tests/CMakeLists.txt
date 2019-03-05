cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(core_test)
enable_testing()

add_executable(core_tests test_core_model.cpp)
add_compile_options(-Wdisabled-macro-expansion)
target_include_directories(
    core_tests
    PRIVATE ${BOOST_INCLUDE_DIRS}
    )
find_package(Boost 1.69 REQUIRED filesystem system regex thread log)
target_link_libraries(core_tests ${Boost_LIBRARIES})
add_definitions(-DBOOST_LOG_DYN_LINK)
set(GCC_)

include_directories(../src)
link_directories(../src)
target_link_libraries(core_tests lvl_one)

add_test(
    NAME test_core_model
    COMMAND $<TARGET_FILE:core_tests>
)

