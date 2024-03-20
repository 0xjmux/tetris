# extern CMakeLists

#add_subdirectory(Unity)

add_library(Unity STATIC
    Unity/src/unity.c
)

target_include_directories(Unity PUBLIC
    Unity/src
)

add_library(ini STATIC
    inih/ini.c
    inih/ini.h
)

target_include_directories(ini PUBLIC 
    inih
)
