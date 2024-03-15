# CMakeLists for tetris game library

#cmake_minimum_required(VERSION 3.16)
#project(tetris VERSION 0.1 LANGUAGES C)


add_library(tetris STATIC tetris.c)

target_include_directories(tetris PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)

 
# include_directories(${PROJECT_SOURCE_DIR})
