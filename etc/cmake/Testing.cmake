# Switch for test cases
option(BUILD_TESTS "Build tests" ON)

if (BUILD_TESTS)
    find_package(Boost 1.48 REQUIRED COMPONENTS unit_test_framework)
    link_directories(${Boost_LIBRARY_DIRS})

    enable_testing()
    
    # create Test.h file which must be included by all testbench files
    if (EXISTS "${PROJECT_SOURCE_DIR}/src/${PROJECT_INSTALL_NAME}")
        configure_file(${PROJECT_TEMPLATES_DIR}/Test.h.in ${PROJECT_BINARY_DIR}/src/${PROJECT_INSTALL_NAME}/Test.h)
    endif()
endif()

##
## This macro adds a testcase to the testsuite which is run by the command 'make test'.
## All test executables are stored in ${PROJECT_BIN_DIR}.
## The Output path of the log files is stored in Test.h which gets generated above.
##
## PARAM TEST_NAME name of the test and the executable which gets created
## PARAM TEST_FILE name of the cpp file containing the test code
## PARAM DEPENDENT_LIBRARIES the libraries which must be linked to the testcase executable
##
macro(project_add_test TEST_NAME TEST_FILE DEPENDENT_LIBRARIES)
    if (BUILD_TESTS)
        message(STATUS "    Building test ${TEST_NAME}")
        add_executable(${TEST_NAME} ${TEST_FILE})
        executable_settings(${TEST_NAME})
        if (NOT OS_WIN)
            target_link_libraries(${TEST_NAME} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} ${DEPENDENT_LIBRARIES})
        endif()
        add_test(NAME ${TEST_NAME}
              COMMAND "${PROJECT_BIN_DIR}/${TEST_NAME}" --output_format=XML --log_level=all --report_level=detailed)
    endif()
endmacro()
