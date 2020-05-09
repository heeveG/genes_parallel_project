cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
project(cmake_and_cuda LANGUAGES CXX CUDA)

set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS -pthread)

add_executable(genome_cuda
        main.cu
        file_readers/read_fasta.cpp
        file_readers/read_csv.cpp
        headers/ahocorasick.h
        headers/read_csv.h
        headers/read_fasta.h
        headers/concurrent_q.h
        )

# Request that particles be built with -std=c++11
# As this is a public compile feature anything that links to particles
# will also build with -std=c++11
target_compile_features(genome_cuda PUBLIC cxx_std_11)

# We need to explicitly state that we need all CUDA files in the particle
# library to be built with -dc as the member functions could be called by
# other libraries and executables
set_target_properties(genome_cuda
        PROPERTIES CUDA_SEPARABLE_COMPILATION ON
        )


