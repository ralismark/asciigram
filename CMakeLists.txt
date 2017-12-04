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

add_definitions(-Wall -Wextra -Werror)

#######################
# Project Sources
#######################

add_executable(nc nc/frontend.cpp nc/globals.cpp nc/cursor.cpp nc/modes.cpp nc/clip.cpp nc/help.cpp)
target_link_libraries(nc ncurses)
