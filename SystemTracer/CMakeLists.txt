cmake_minimum_required(VERSION 3.19)
project(OS_proj2)

set(CMAKE_CXX_STANDARD 11)

add_executable(OS_proj2 main.cpp Task.cpp Task.h)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")