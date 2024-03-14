# # CMakeLists for tetris game library

cmake_minimum_required(VERSION 3.16)
project(tetris VERSION 0.1 LANGUAGES C)


file(GLOB PROJECT_SOURCES "*.c" "*.h")

# # add_library(${PROJECT_NAME} STATIC ${PROJECT_SOURCES})
add_library(${PROJECT_NAME} STATIC ${PROJECT_SOURCES})
 
# include_directories(${PROJECT_SOURCE_DIR})
