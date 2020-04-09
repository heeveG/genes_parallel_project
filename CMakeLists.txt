cmake_minimum_required(VERSION 3.15)
project(ac_oneT)

set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS -pthread)
add_executable(ac_oneT main.cpp ahocorasick.cpp)