cmake_minimum_required (VERSION 3.0)

set(CMAKE_CXX_COMPILER "dpcpp")


if (NOT CMAKE_BUILD_TYPE)
    message (STATUS "Default CMAKE_BUILD_TYPE not set using Release with Debug Info")
    set (CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE
            STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel"
            FORCE)
endif()

set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS -pthread)

add_executable (genome main.cpp file_readers/read_fasta.cpp file_readers/read_csv.cpp oneapi/oneapi_parallel.cpp)

target_link_libraries(genome OpenCL sycl)







