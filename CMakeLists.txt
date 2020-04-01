cmake_minimum_required(VERSION 3.8)
project(genes_project VERSION 1.0.0 LANGUAGES CXX)

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

find_package(Boost 1.66 REQUIRED COMPONENTS)

#include("${CMAKE_CURRENT_SOURCE_DIR}/includes/CMakeLists.txt")

#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/data)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/includes)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/genes-exe)

target_include_directories(genes_project PRIVATE ${Boost_INCLUDE_DIRS})

target_include_directories(genes_project PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/../data"
        )

# target_link_libraries(LibrariesModule -lpthread)



#set(output_csv ${CMAKE_CURRENT_SOURCE_DIR}/genes-res/output.csv)
#set(markers_csv ${CMAKE_CURRENT_SOURCE_DIR}/markers.csv)
#
## for adding a custom command to produce an output
#add_custom_command(
#        OUTPUT  ${output_csv}
#        COMMAND genes_project ${markers_csv} ${output_csv}
#        DEPENDS genes_project ${markers_csv}
#)
#
## to generate a file with dependencies
#add_custom_target(process_csv
#        ALL   # means that this custom target is also included in the ALL target
#        DEPENDS ${output_csv}
#        ) # make || make all || make process_csv
