cmake_minimum_required(VERSION 3.12)
project(warship)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

find_package(SFML 2.5 COMPONENTS window system REQUIRED)
find_package(FMODL)
#add_library(FMOD SHARED IMPORTED)

add_executable(warship
        src/main.cpp
        src/Warship.cpp src/Warship.h)

target_link_libraries(warship
        sfml-window
        sfml-system
        FMODL)

# copy resources to exec directory for testing
add_custom_command(TARGET warship PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resource $<TARGET_FILE_DIR:warship>/resource)