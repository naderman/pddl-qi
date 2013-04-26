# determine number of available CPUs
if(NOT DEFINED PROCESSOR_COUNT)
    if(CMAKE_MAJOR_VERSION EQUAL 2 AND CMAKE_MINOR_VERSION EQUAL 8 AND CMAKE_PATCH_VERSION GREATER 4)
        include(ProcessorCount)
        ProcessorCount(PROCESSOR_COUNT)
    else()
        # Unknown:
        set(PROCESSOR_COUNT 0)

        # Linux:
        set(cpuinfo_file "/proc/cpuinfo")
        if(EXISTS "${cpuinfo_file}")
            file(STRINGS "${cpuinfo_file}" procs REGEX "^processor.: [0-9]+$")
            list(LENGTH procs PROCESSOR_COUNT)
        endif()

        # Mac:
        if(APPLE)
            find_program(cmd_sys_pro "system_profiler")
            if(cmd_sys_pro)
                execute_process(COMMAND ${cmd_sys_pro} OUTPUT_VARIABLE info)
                string(REGEX REPLACE "^.*Total Number Of Cores: ([0-9]+).*$" "\\1"
                    PROCESSOR_COUNT "${info}")
            endif()
        endif()

        # Windows:
        if(WIN32)
            set(PROCESSOR_COUNT "$ENV{NUMBER_OF_PROCESSORS}")
        endif()
    endif()
    MESSAGE (STATUS "        Number of CPUs: ${PROCESSOR_COUNT}")
endif()

macro(executable_settings_ext EXECUTABLE_NAME CFLAGS LDFLAGS)
    # Like executable_settings, but allows additional compiler-flags and linker-flags
    message(STATUS "        Configuring executable ${EXECUTABLE_NAME}.")
    compilation_settings()

    message(STATUS "        Using compiler flags (CXX_FLAGS): ${CXX_FLAGS} ${CFLAGS}")
    message(STATUS "        Using linker flags: ${LDFLAGS}")
    set_target_properties(${EXECUTABLE_NAME} PROPERTIES
        COMPILE_FLAGS "${CXX_FLAGS} ${CFLAGS}"
        LINK_FLAGS "${LDFLAGS}"
        LIBRARY_OUTPUT_DIRECTORY "${PROJECT_LIB_DIR}"
        ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_ARCHIVE_DIR}"
        RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BIN_DIR}"
    )

    message(STATUS "        RUNTIME_OUTPUT_DIRECTORY: " ${PROJECT_BIN_DIR})

    if (OS_LINUX)
        get_target_property(EXECUTABLE_FILE ${EXECUTABLE_NAME} LOCATION)
        if (${CMAKE_BUILD_TYPE} MATCHES "Release")
            MESSAGE(STATUS "        RELEASE_MODE: stripping binaries")
            add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
                               COMMAND strip ARGS --strip-unneeded ${EXECUTABLE_FILE})
        endif()
    endif()

    executable_install(${EXECUTABLE_NAME})
endmacro()

macro(executable_settings EXECUTABLE_NAME)
    executable_settings_ext(${EXECUTABLE_NAME} "" "")
endmacro()

macro(library_base_settings_begin LIB_NAME LIB_VERSION LIB_SOVERSION HEADERS)
    message(STATUS "        Configuring shared library ${LIB_NAME} version ${LIB_VERSION}.")
    compilation_settings()

    if(NOT OS_WIN)
        set(CXX_FLAGS "${CXX_FLAGS} -fPIC -D_REENTRANT")
    else()
        add_definitions(-D_USE_MATH_DEFINES)
    endif()
endmacro()

macro(library_base_settings_end LIB_NAME LIB_VERSION LIB_SOVERSION HEADERS)
    set_target_properties(${LIB_NAME} PROPERTIES
        VERSION   ${LIB_VERSION}
        SOVERSION ${LIB_SOVERSION})

    message(STATUS "        Using compiler flags (CXX_FLAGS): " ${CXX_FLAGS})
    set_target_properties(${LIB_NAME} PROPERTIES
        COMPILE_FLAGS "${CXX_FLAGS}"
        LIBRARY_OUTPUT_DIRECTORY "${PROJECT_LIB_DIR}"
        ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_ARCHIVE_DIR}"
        RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BIN_DIR}"
    )

    message(STATUS "        LIBRARY_OUTPUT_DIRECTORY: " ${PROJECT_LIB_DIR})
    message(STATUS "        ARCHIVE_OUTPUT_DIRECTORY: " ${PROJECT_ARCHIVE_DIR})
    message(STATUS "        RUNTIME_OUTPUT_DIRECTORY: " ${PROJECT_BIN_DIR})

    if (OS_LINUX)
        get_target_property(LIB_FILE ${LIB_NAME} LOCATION)
        if (${CMAKE_BUILD_TYPE} MATCHES "Release")
            MESSAGE(STATUS "        RELEASE_MODE: stripping binaries")
            add_custom_command(TARGET ${LIB_NAME} POST_BUILD
                               COMMAND strip ARGS --strip-unneeded ${LIB_FILE})
        endif()
    endif()

    set(SLICE_DEPENDS "")
    foreach(SLICE_DEPEND ${SLICE_DEPENDS_DIRTY})
        string(LENGTH ${SLICE_DEPEND} SLICE_DEPEND_LENGTH)
        math(EXPR SLICE_DEPEND_LENGTH "${SLICE_DEPEND_LENGTH}-1")
        string(SUBSTRING ${SLICE_DEPEND} 1 ${SLICE_DEPEND_LENGTH} SLICE_DEPEND)
        string(STRIP ${SLICE_DEPEND} SLICE_DEPEND)
        list(APPEND SLICE_DEPENDS ${SLICE_DEPEND})
    endforeach()

    string(REPLACE "${CMAKE_SOURCE_DIR}/" "" HEADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

    library_install("${LIB_NAME}" "${LIB_VERSION}" "${LIB_SOVERSION}" "${HEADERS}" "${HEADER_DIR}")
endmacro()

macro(library_settings LIB_NAME LIB_VERSION LIB_SOVERSION HEADERS)
    library_base_settings_begin("${LIB_NAME}" "${LIB_VERSION}" "${LIB_SOVERSION}" "${HEADERS}")

    if (OS_WIN)
        add_definitions(-DWINDOWS_EXPORTS)
    endif()

    library_base_settings_end("${LIB_NAME}" "${LIB_VERSION}" "${LIB_SOVERSION}" "${HEADERS}")
endmacro()


macro(compilation_settings)
    # Ice uses long long in C++ which -pedantic turns into an error instead of
    # a warning - we turn it off entirely
    # would really like to use -pedantic here, but Ice breaks it
    set(GENERAL_FLAGS "-Wall -Wextra -Wno-long-long -Wno-unused-parameter")

    if (NOT OS_WIN)
        if (CMAKE_BUILD_TYPE STREQUAL Release)
            set(CXX_FLAGS "-O2 ${GENERAL_FLAGS}")
        else()
            set(CXX_FLAGS "-g ${GENERAL_FLAGS}")
        endif()
    endif()

    # enable multipthreaded compilation on visual studio
    if (OS_WIN AND MSVC)
        add_definitions(/MP)
    endif()

endmacro()
