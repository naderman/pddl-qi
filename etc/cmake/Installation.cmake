# macros for handling installation instructions

macro(export_project)
    # Export the package for use from the build-tree
    # (this registers the build-tree with a global CMake-registry)
    export(PACKAGE ${PROJECT_NAME})

    # all libs & exes are added to this
    set(PROJECT_LIBS "" CACHE INTERNAL "" FORCE)
    set(PROJECT_EXES "" CACHE INTERNAL "" FORCE)
endmacro()

macro(install_project)
    set(PROJECT_VERSION "")
    
    set(INSTALL_TARGETS True)
    if("${PROJECT_EXES}" STREQUAL "")
        if("${PROJECT_LIBS}" STREQUAL "")
            set(INSTALL_TARGETS False)
        endif()
    endif()
    
    # Add targets to the build-tree export set
    # first the string must be split into a list before it can be exported
    string(REPLACE " " ";" TARGET_LIST "${PROJECT_LIBS} ${PROJECT_EXES}")
    
    if(INSTALL_TARGETS)
        export(TARGETS ${TARGET_LIST}
               FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}LibraryDepends.cmake")
    endif()

    # Create a ${PROJECT_NAME}Config.cmake file for the use from the build tree
    set(PROJECT_INCLUDE_DIRS "${PROJECT_INCLUDE_DIRECTORIES}")
    set(PROJECT_LIB_DIR "${PROJECT_BINARY_DIR}/lib")
    set(PROJECT_BIN_DIR "${PROJECT_BINARY_DIR}/bin")
    set(PROJECT_TEMPLATES_DIR "${PROJECT_ETC_DIR}/templates")

    configure_file(${PROJECT_TEMPLATES_DIR}/cmake/ProjectBuildTreeSettings.cmake.in
                   "${PROJECT_BINARY_DIR}/${PROJECT_NAME}BuildTreeSettings.cmake"
                   @ONLY)

    # Create a ${PROJECT_NAME}Config.cmake file for the use from the install tree
    set(PROJECT_INCLUDE_DIRS "include")
    set(PROJECT_LIB_DIR "lib")
    set(PROJECT_BIN_DIR "bin")
    set(PROJECT_INSTALL_CMAKE_DIR "share/${PROJECT_NAME}/cmake")
    set(PROJECT_INSTALL_TEMPLATES_DIR "share/${PROJECT_NAME}/templates")

    file(RELATIVE_PATH RELATIVE_BASE_DIR
         "${CMAKE_INSTALL_PREFIX}/share/cmake/${PROJECT_NAME}"
         "${CMAKE_INSTALL_PREFIX}")
    configure_file(${PROJECT_TEMPLATES_DIR}/cmake/ProjectConfig.cmake.in
                   "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
                   @ONLY)
    configure_file(${PROJECT_TEMPLATES_DIR}/cmake/ProjectConfigVersion.cmake.in
                   "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
                   @ONLY)

    # Install the export set for use with the install-tree (belongs to the files generated above)
    if(INSTALL_TARGETS)
        install(EXPORT "${PROJECT_NAME}LibraryDepends"
                DESTINATION "share/cmake/${PROJECT_NAME}"
                COMPONENT dev)
    endif()

    # Install the Config files into the install tree
    install(FILES
            "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
            "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
            DESTINATION "share/cmake/${PROJECT_NAME}"
            COMPONENT dev)

endmacro()

macro(executable_install EXE_NAME)
    install(TARGETS ${EXE_NAME}
        EXPORT ${PROJECT_NAME}LibraryDepends
        RUNTIME DESTINATION bin
    )

    if (PROJECT_EXES)
        SET(PROJECT_EXES "${PROJECT_EXES} ${EXE_NAME}" CACHE INTERNAL "" FORCE)
    else()
        set(PROJECT_EXES "${EXE_NAME}" CACHE INTERNAL "" FORCE)
    endif()

endmacro()

macro(library_install LIB_NAME LIB_VERSION LIB_SOVERSION LIB_HEADERS HEADER_DIR)
    foreach (HEADER ${LIB_HEADERS})
        string(REPLACE "${CMAKE_SOURCE_DIR}/${HEADER_DIR}" "" HEADER "${HEADER}")
        get_filename_component(EXTRA_DIR "${HEADER}" PATH)

        install(FILES ${LIB_HEADERS} DESTINATION "include/${PROJECT_INSTALL_NAME}/${HEADER_DIR}/${EXTRA_DIR}")
    endforeach()

    if (OS_WIN)
        install(TARGETS ${LIB_NAME}
            DESTINATION lib)
    else()
        install(TARGETS ${LIB_NAME}
            EXPORT ${PROJECT_NAME}LibraryDepends
            RUNTIME DESTINATION bin
            LIBRARY DESTINATION lib
            PUBLIC_HEADER DESTINATION include/${PROJECT_INSTALL_NAME}/${LIB_NAME}
        )
    endif()

    if (PROJECT_LIBS)
        SET(PROJECT_LIBS "${PROJECT_LIBS} ${LIB_NAME}" CACHE INTERNAL "" FORCE)
    else()
        set(PROJECT_LIBS "${LIB_NAME}" CACHE INTERNAL "" FORCE)
    endif()
endmacro()

