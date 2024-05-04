# CMakeLists for tetris game library



add_library(tetris STATIC tetris.c)

target_include_directories(tetris PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)

##### OPTIONAL DEBUG FLAG ######
# This flag gates all print statements inside tetris.c. Extremely helpful for debugging, 
#   but shoudln't be present in release builds
OPTION(TETRIS_DEBUG_T_MACRO "Enable Debug logging from inside tetris" OFF)
IF(TETRIS_DEBUG_T_MACRO)
    target_compile_definitions(tetris PUBLIC DEBUG_T=1)
ENDIF(TETRIS_DEBUG_T_MACRO)
 
# include_directories(${PROJECT_SOURCE_DIR})
