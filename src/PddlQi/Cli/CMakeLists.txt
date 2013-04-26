set_target("PddlQi")

set(BUILD TRUE)

build_if(BUILD "library disabled")

if (BUILD)
    set(EXE_NAME "pddlqi")

    set(LIBS "")

    set(EXE_FILES Cli.cpp
        Main.cpp
    )

    set(LIB_HEADERS Cli.h
    )

    add_executable(${EXE_NAME} ${EXE_FILES})

    executable_settings("${EXE_NAME}")
    target_link_libraries(${EXE_NAME} ${LIBS})

    #add_subdirectory(test)
endif()
