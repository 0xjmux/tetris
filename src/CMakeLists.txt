# src CMakeLists - 
#   driver code added only for main executable


# don't put binaries in subdirectories under build/
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

add_executable(tetris_driver
    driver_tetris.c 
    utils.c
)
target_include_directories(tetris_driver PUBLIC ${PROJECT_SOURCE_DIR}/include)
# message("building driver with proj source dir ${PROJECT_SOURCE_DIR}")

find_library(NCURSES_FOUND ncurses REQUIRED)

OPTION(DEBUG_WINDOW "Enable debug window with game state information in ncurses" OFF)
IF(DEBUG_WINDOW)
    target_compile_definitions(tetris_driver PUBLIC DEBUG_T_WIN=1)
ENDIF()


# find ini library (needed for saving game state to disk)
OPTION(INI_LIB_INCLUDE_OPTION "Include inih library for saving game state to disk" ON)
IF(INI_LIB_INCLUDE_OPTION)
    # find_library(INIH_FOUND ${PROJECT_SOURCE_DIR}/extern/inih/ini.c REQUIRED)
    target_link_libraries(tetris_driver ncurses tetris ini)
ELSE()
    target_link_libraries(tetris_driver ncurses tetris)
ENDIF()

