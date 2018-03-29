cmake_minimum_required(VERSION 3.5)

project(asciigram)

#######################
# Project Setup
#######################

# C++14 support
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address")
set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address")

# need this for multithreading
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

# Use the package PkgConfig to detect GTK+ headers/library files
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})
add_definitions(${GTK3_CFLAGS_OTHER})

add_definitions(-Wall -Wextra -Werror)

#######################
# Project Sources
#######################

add_library(sysclip sysclip.cpp)
target_link_libraries(sysclip ${GTK3_LIBRARIES})

add_executable(nc nc/frontend.cpp nc/globals.cpp nc/cursor.cpp nc/modes.cpp nc/clip.cpp nc/help.cpp)
target_link_libraries(nc ncurses sysclip)
