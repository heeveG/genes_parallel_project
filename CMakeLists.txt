cmake_minimum_required(VERSION 3.10)
project(ac_oneT VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

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

add_library(LibrariesModule ${CMAKE_CURRENT_SOURCE_DIR}/src/util/ahocorasick.cpp)
# target_link_libraries(LibrariesModule -lpthread)

find_package(Boost 1.65 REQUIRED COMPONENTS)
include_directories( ${Boost_INCLUDE_DIR})

add_executable(ac_oneT ${CMAKE_CURRENT_SOURCE_DIR}/src/main/app.cpp)

target_link_libraries(ac_oneT LibrariesModule)

set(output_csv ${CMAKE_CURRENT_SOURCE_DIR}/../data/output.csv)
set(markers_csv ${CMAKE_CURRENT_SOURCE_DIR}/../data/markers.csv)

# for adding a custom command to produce an output
add_custom_command(
        OUTPUT  ${output_csv}
        COMMAND ac_oneT ${markers_csv} ${output_csv}
        DEPENDS ac_oneT ${markers_csv}
)

# to generate a file with dependencies
add_custom_target(process_csv
        ALL   # means that this custom target is also included in the ALL target
        DEPENDS ${output_csv}
        ) # make || make all || make process_csv

target_include_directories(ac_oneT PUBLIC
        "${PROJECT_SOURCE_DIR}/../data"
        )