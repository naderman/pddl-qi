cmake_minimum_required(VERSION 2.8)

get_filename_component(PROJECT_CMAKE_DIR "${PROJECT_SOURCE_DIR}/etc/cmake" ABSOLUTE)
get_filename_component(PROJECT_TEMPLATES_DIR "${PROJECT_SOURCE_DIR}/etc/templates" ABSOLUTE)

include(${PROJECT_CMAKE_DIR}/Setup.cmake)

setup_project(PddlQi)

add_subdirectory(etc)
add_subdirectory(src)

install_project()

