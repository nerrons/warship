cmake_minimum_required(VERSION 3.12)
project(worshcpp)

set(CMAKE_CXX_STANDARD 14)

include_directories(
        ${PROJECT_SOURCE_DIR}/inc/
)

link_directories(
        ${PROJECT_SOURCE_DIR}/deps/
)

add_executable(
        worshcpp
        src/main.cpp
        src/Warship.cpp src/Warship.h)
