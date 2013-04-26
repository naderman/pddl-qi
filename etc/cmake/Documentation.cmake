find_package(Doxygen)

if (DOXYGEN_FOUND)
    set(DOXYGEN_TEMPLATE_DIR "${PROJECT_TEMPLATES_DIR}/doxygen")
    set(DOXYGEN_CONFIG_FILE "${DOXYGEN_DOCUMENTATION_DIR}/${PROJECT_NAME}.Doxyfile")
    set(DOXYGEN_PROPERTIES_DOC_FILE "${DOXYGEN_DOCUMENTATION_DIR}/PropertiesDocumentation.cmake")


    message(STATUS "Documentation: Generating Doxygen files")
    CONFIGURE_FILE(
       "${DOXYGEN_TEMPLATE_DIR}/Project.Doxyfile.in"
       "${DOXYGEN_CONFIG_FILE}"
        @ONLY)

    set(DOXYGEN_TEMPLATE_FILES
        ${DOXYGEN_TEMPLATE_DIR}/Doxygen.css
        ${DOXYGEN_TEMPLATE_DIR}/Footer.html
        ${DOXYGEN_TEMPLATE_DIR}/Header.html
        ${DOXYGEN_TEMPLATE_DIR}/DoxygenLayout.xml)
    file(COPY ${DOXYGEN_TEMPLATE_FILES} DESTINATION ${DOXYGEN_DOCUMENTATION_LAYOUT_DIR})

    CONFIGURE_FILE(
       "${PROJECT_TEMPLATES_DIR}/cmake/PropertiesDocumentation.cmake.in"
       "${DOXYGEN_PROPERTIES_DOC_FILE}"
        @ONLY)

    add_custom_target(doc
                      COMMAND ${CMAKE_COMMAND} -P "${DOXYGEN_PROPERTIES_DOC_FILE}"
                      COMMAND ${DOXYGEN_EXECUTABLE} "${DOXYGEN_CONFIG_FILE}"
                      # DEPENDS ProjectName.Doxyfile
                      WORKING_DIRECTORY "${DOXYGEN_DOCUMENTATION_DIR}"
                      COMMENT "Generating documentation for ${PROJECT_NAME}")
else (DOXYGEN_FOUND)
    message(STATUS "Doxygen not found: the documentation can not be generated for ${PROJECT_NAME}")
endif (DOXYGEN_FOUND)
