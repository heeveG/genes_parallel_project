cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_COMPILER "dpcpp")

set(CMAKE_CXX_STANDARD 17)
find_package(Threads)

if (WIN32)
    find_library( LibArchive NAMES libarchive.dll )
elseif(APPLE)
    find_library( LibArchive NAMES libarchive.dylib )
else()
    find_library( LibArchive NAMES libarchive.so )
endif ()

add_executable(genome main.cpp file_readers/read_fasta.cpp file_readers/read_csv.cpp oneapi/oneapi_parallel.cpp)

target_link_libraries(genome OpenCL sycl ${LibArchive} ${CMAKE_THREAD_LIBS_INIT})







