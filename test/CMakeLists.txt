
# don't put binaries in subdirectories under build/
#SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

add_executable(test_tetris
    suite_1.c
    tetris_test_helpers.c
)

target_link_libraries(test_tetris
    tetris
    Unity
)

add_test(suite_1_test test_tetris)
