cmake_minimum_required(VERSION 3.5)

project(asciigram)

#######################
# Project Setup
#######################

# C++14 support
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#######################
# Project Sources
#######################

add_executable(main main.cpp)

add_executable(nc nc/frontend.cpp nc/globals.cpp nc/cursor.cpp nc/modes.cpp nc/clip.cpp)
target_link_libraries(nc ncurses)
