cmake_minimum_required(VERSION 2.8)

macro(setup_project NAME)
    set(PROJECT_NAME "${NAME}")
    set(PROJECT_INSTALL_NAME "${NAME}")
    if(NOT "${ARGV1}" STREQUAL "")
        set(PROJECT_INSTALL_NAME "${ARGV1}")
    endif()
    project("${NAME}" CXX)

    # Utility functions and macros
    include(${PROJECT_CMAKE_DIR}/CMakeUtil.cmake)
    # Identify OS and setup
    include(${PROJECT_CMAKE_DIR}/SetupOs.cmake)
    # Setup directories and paths
    include(${PROJECT_CMAKE_DIR}/SetupDirs.cmake)

    # Boost
    MESSAGE(STATUS "Looking for Boost")
    set(Boost_USE_MULTITHREADED ON)
    set(Boost_ADDITIONAL_VERSIONS "1.48.0" "1.48")
    set(Boost_NO_SYSTEM_PATHS ON) # prevent mixing different Boost installations
    find_package(Boost 1.46)
    include_directories(${Boost_INCLUDE_DIR})
    MESSAGE(STATUS "Boost found at ${Boost_INCLUDE_DIR}")

    include(${PROJECT_CMAKE_DIR}/CompileSettings.cmake)
    include(${PROJECT_CMAKE_DIR}/Installation.cmake)
    include(${PROJECT_CMAKE_DIR}/Testing.cmake)
    include(${PROJECT_CMAKE_DIR}/Documentation.cmake)

    export_project()

    # This variable is needed in Installation.cmake for setting
    # PROJECT_INCLUDE_DIRS
    set(PROJECT_INCLUDE_DIRECTORIES
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_BINARY_DIR}/src
    )

    include_directories(${PROJECT_INCLUDE_DIRECTORIES})

    link_directories(${PROJECT_LIB_DIR})
endmacro()

