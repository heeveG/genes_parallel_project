cmake_minimum_required(VERSION 3.8)
project(genes_project VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi")

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # Clang, AppleClang
    set( CMAKE_CXX_FLAGS "-Ofast" )
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # GCC
    set( CMAKE_CXX_FLAGS "-O3 -Wall -Wextra" )
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
    # Intel ++
    set( CMAKE_CXX_FLAGS "-fast" )
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # MSVC
    set( CMAKE_CXX_FLAGS "-O2" )
endif()

set (Boost_USE_STATIC_LIBS OFF)
set (Boost_USE_MULTITHREAD ON)
find_package(ZLIB)
find_package(Boost 1.66 REQUIRED COMPONENTS)

message(${Boost_VERSION})

#include("${CMAKE_CURRENT_SOURCE_DIR}/includes/CMakeLists.txt")

#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/data)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/includes)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/genes-exe)


target_include_directories(genes_project PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(genes_project PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/../data"
        )