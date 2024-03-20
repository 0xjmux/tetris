
# don't put binaries in subdirectories under build/
#SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

add_executable(test_tetris
    suite_1.c
    tetris_test_helpers.c
)

OPTION(TETRIS_UNIT_TEST_MACRO "Print gamelog to stdout (for CI)" OFF) # disabled by default
IF(TETRIS_UNIT_TEST_MACRO)
    target_compile_definitions(test_tetris PUBLIC TETRIS_UNIT_TEST_DEF=1)
ENDIF(TETRIS_UNIT_TEST_MACRO)

target_link_libraries(test_tetris
    tetris
    Unity
)

add_test(suite_1_test test_tetris)
