
# don't put binaries in subdirectories under build/
#SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

SET(TETRIS_TEST_FILES tetris_test_helpers.c ${PROJECT_SOURCE_DIR}/src/utils.c)

add_executable(test_tetris suite_1.c ${TETRIS_TEST_FILES} )
# add_executable(test2_tetris suite_2.c ${TETRIS_TEST_FILES} )

    # tetris_test_helpers.c
    # ${PROJECT_SOURCE_DIR}/src/utils.c
include_directories(test_tetris PUBLIC ${PROJECT_SOURCE_DIR}/include)


##### OPTIONAL BUILD FLAGS ######
OPTION(TETRIS_PRINT_BOARD_IN_TESTS_MACRO "Print board images to console during testing" OFF)
IF(TETRIS_PRINT_BOARD_IN_TESTS_MACRO)
    target_compile_definitions(test_tetris PUBLIC TEST_PRINT_BOARD=1)
ENDIF(TETRIS_PRINT_BOARD_IN_TESTS_MACRO)

# CI builds run from {PROJ_ROOT}/build, whereas I usually run the binary from
#  ${PROJ_ROOT} itself
OPTION(TETRIS_UNIT_TEST_CI "CI-specific path options" OFF) # disabled by default
IF(TETRIS_UNIT_TEST_CI)
    target_compile_definitions(test_tetris PUBLIC TETRIS_UNIT_TEST_CI=1)
ENDIF(TETRIS_UNIT_TEST_CI)

OPTION(TETRIS_UNIT_TEST_MACRO "Print gamelog to stdout (for CI)" OFF) # disabled by default
IF(TETRIS_UNIT_TEST_MACRO)
    target_compile_definitions(test_tetris PUBLIC TETRIS_UNIT_TEST_DEF=1)
ENDIF(TETRIS_UNIT_TEST_MACRO)
##### END OPTIONAL BUILD FLAGS ######

target_link_libraries(test_tetris
    tetris
    Unity
    ini
)

add_test(suite_1_test test_tetris)
# add_test(suite_2_test test_tetris)
