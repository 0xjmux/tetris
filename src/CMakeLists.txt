# src CMakeLists - 
#   driver code added only for main executable


# don't put binaries in subdirectories under build/
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

add_executable(tetris_driver
    driver_tetris.c 
    display.c
)
target_include_directories(tetris_driver PUBLIC ${PROJECT_SOURCE_DIR}/include)
# message("building driver with proj source dir ${PROJECT_SOURCE_DIR}")

find_library(NCURSES_FOUND ncurses REQUIRED)
target_link_libraries(tetris_driver ncurses tetris)

