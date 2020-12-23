cmake_minimum_required(VERSION 3.17)
project(maze)

set(CMAKE_CXX_STANDARD 14)
set(OpenGlLinkers -lglut -lGLU -lGL)

add_executable(maze main.cpp functions.h constants.h maps/dev.h collision.h)
target_link_libraries(maze ${OpenGlLinkers})