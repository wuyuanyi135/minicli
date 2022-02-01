cmake_minimum_required(VERSION 3.21)
project(minicli)

set(CMAKE_CXX_STANDARD 17)
include_directories(include)

OPTION(MINICLI_ENABLE_EXCEPTION "Enable C++ Exception handling" OFF)
IF(MINICLI_ENABLE_EXCEPTION)
    ADD_DEFINITIONS(-DMINICLI_ENABLE_EXCEPTION)
ENDIF(MINICLI_ENABLE_EXCEPTION)

add_library(minicli src/command.cpp src/checker.cpp src/app.cpp)

add_executable(unit_test tests/test_main.cpp)
target_link_libraries(unit_test minicli)

add_executable(example_simple examples/simple.cpp)
target_link_libraries(example_simple minicli)