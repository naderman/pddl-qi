
file(GLOB CMAKE_FILES *.cmake)
install(FILES ${CMAKE_FILES} DESTINATION share/${PROJECT_NAME}/cmake/)
