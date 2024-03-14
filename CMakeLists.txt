#idf_component_register(SRCS "driver_tetris.c" INCLUDE_DIRS "include")


# Rewritten CMakeLists.txt for tetris game

cmake_minimum_required(VERSION 3.16)
project(tetris_driver VERSION 0.1 LANGUAGES C)
 
set(CMAKE_C_STANDARD 11)

# for future; option to build for terminal use so nothing is included needlessly
#option(BUILD_FOR_TERM "Compile game with driver for testing on normal desktop platforms" NO)
#if (BUILD_FOR_TERM)
#endif()

# include libtetris game library
# add_subdirectory(libtetris)

file(GLOB PROJECT_SOURCE "*.c" "src/*.c")
file(GLOB PROJECT_HEADER "include/*.h" "*.h")

# set up libtetris library
add_library(tetris STATIC tetris/tetris.c tetris/tetris.h)
target_include_directories(tetris PUBLIC tetris)

add_executable(tetris_driver ${PROJECT_SOURCE})
# include_directories(${PROJECT_SOURCE_DIR}/include)
target_include_directories(tetris_driver PUBLIC ${PROJECT_SOURCE_DIR}/include)


# ncurses for GUI
find_library(NCURSES_FOUND ncurses REQUIRED)
target_link_libraries(${PROJECT_NAME} ncurses tetris)
#if(NCURSES_FOUND )
#  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ncurses)
#  set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ncurses)
#else() 
#  message(ERROR "NCURSES NOT FOUND!")
#endif(NCURSES_FOUND )




# platform-specific compilation flags
if(UNIX)
  message(INFO "LINUX DETECTED")
  set(CMAKE_C_FLAGS_DEBUG "-ggdb")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")

  # it didn't like the normal way of doing it so here we are
  set(CMAKE_C_FLAGS "-Wall -g -ggdb")

#   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
elseif(WIN32)
  add_definitions(-D_WIN32_WINNT=0x0600)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
#   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -wd4200")
#   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2")
else()
    message(ERROR "PLATFORM NOT FOUND")
endif()
