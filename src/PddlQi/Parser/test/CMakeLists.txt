
SET(LIBS ${LIBS} PddlQiParser)

project_add_test(ParserTest ParserTest.cpp "${LIBS}")
