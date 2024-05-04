# ESP-IDF component line
if(ESP_PLATFORM)
idf_component_register(SRCS "tetris/tetris.c"
                    INCLUDE_DIRS "tetris")

return()
message(FATAL_ERROR "should not reach during idf build!!!")
endif()



#cmake -DTARGET_GROUP=test {path-to-source-tree}
# make

# SET PROJECT INFO AND COMPILATION FLAGS
########################################
cmake_minimum_required(VERSION 3.16)
project(tetris_driver VERSION 0.1 LANGUAGES C)
 
set(CMAKE_C_STANDARD 11)
  
### Require out-of-source buildsw
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
    message(FATAL_ERROR "You cannot build in a source directory (or any directory with "
                        "CMakeLists.txt file). Please make a build subdirectory. Feel free to "
                        "remove CMakeCache.txt and CMakeFiles.")
endif()

# platform-specific compilation flags
if(UNIX)
  message(INFO "LINUX DETECTED")
  set(CMAKE_C_FLAGS_DEBUG "-ggdb -O0")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")

  # it didn't like the normal way of doing it so here we are
  # set(CMAKE_C_FLAGS "-Wall -Wextra -g -ggdb -O0")
  set(CMAKE_C_FLAGS "-Wall -g -ggdb -O0")

#   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
elseif(WIN32)
  add_definitions(-D_WIN32_WINNT=0x0600)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
#   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -wd4200")
#   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2")
else()
    message(ERROR "PLATFORM NOT FOUND")
endif()
########################################

file(GLOB PROJECT_SOURCE "*.c" "src/*.c")
file(GLOB PROJECT_HEADER "include/*.h" "*.h")

# don't put binaries in subdirectories under build/
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

# include libtetris game library
add_subdirectory(tetris)

# set up libtetris library
#add_library(tetris STATIC tetris/tetris.c tetris/tetris.h)
#target_include_directories(tetris PUBLIC tetris)

# add_executable(tetris_driver ${PROJECT_SOURCE})
# include_directories(${PROJECT_SOURCE_DIR}/include)


# ncurses for GUI
#if(NCURSES_FOUND )
#  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ncurses)
#  set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ncurses)
#else() 
#  message(ERROR "NCURSES NOT FOUND!")
#endif(NCURSES_FOUND )



# this section added from cmake-unity-test
set(TARGET_GROUP production CACHE STRING "Group to build")
# add_subdirectory(module_a)

if(TARGET_GROUP STREQUAL production)
  add_subdirectory(extern)
  include_directories(include)
  add_subdirectory(src)
elseif(TARGET_GROUP STREQUAL test)
  enable_testing()
  include(CTest)

  add_subdirectory(extern)
  add_subdirectory(test)
else()
  message(FATAL_ERROR "Given TARGET_GROUP unknown")
endif()




