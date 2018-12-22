cmake_minimum_required(VERSION 3.12)
project(warship)

set(CMAKE_CXX_STANDARD 14)

include_directories(
        ${PROJECT_SOURCE_DIR}/inc/
)

link_directories(
        ${PROJECT_SOURCE_DIR}/deps/
)

find_package(SFML 2.5 COMPONENTS window system REQUIRED)

add_library(fmod SHARED IMPORTED)
set_property(TARGET fmod
        PROPERTY IMPORTED_LOCATION "/Users/Nerrons/Dropbox/the-ultimate-truth/cpp/warshipped/deps/libfmodL.dylib")

add_executable(warship
        src/main.cpp
        src/Warship.cpp src/Warship.h)

target_link_libraries(warship
        sfml-window
        sfml-system
        fmod)

add_custom_command(TARGET warship PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resource $<TARGET_FILE_DIR:warship>/resource)