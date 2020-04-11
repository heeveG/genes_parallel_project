cmake_minimum_required(VERSION 3.15)
project(ac_oneT)

FIND_PACKAGE( Boost  )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS -pthread)
add_executable(ac_oneT main.cpp src/ahocorasick.cpp file_readers/read_fasta.cpp file_readers/read_csv.cpp)