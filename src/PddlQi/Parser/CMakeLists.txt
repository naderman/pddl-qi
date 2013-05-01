set_target("PddlQi")

set(BUILD TRUE)

build_if(BUILD "library disabled")

if (BUILD)
    set(LIB_NAME       PddlQiParser)
    set(LIB_VERSION    0.1.0)
    set(LIB_SOVERSION  0)

    set(LIBS "")

    set(LIB_FILES Parser.cpp
    )

    set(LIB_HEADERS PddlGrammar.h
                    Parser.h
    )

    add_library(${LIB_NAME} SHARED ${LIB_FILES} ${LIB_HEADERS})

    library_settings("${LIB_NAME}" "${LIB_VERSION}" "${LIB_SOVERSION}" "${LIB_HEADERS}")
    target_link_libraries(${LIB_NAME} ${LIBS})

    add_subdirectory(test)
endif()
