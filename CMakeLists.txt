cmake_minimum_required(VERSION 3.15)
project(ac_oneT)

set(CMAKE_CXX_STANDARD 17)

add_executable(ac_oneT src/main/main.cpp src/util/ahocorasick.cpp)