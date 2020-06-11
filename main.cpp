#include "headers/read_csv.h"
#include "headers/read_fasta.h"
#include "headers/oneapi_parallel.h"
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <atomic>
#include <string.h>


inline std::chrono::high_resolution_clock::time_point get_current_time_fenced() {
    std::atomic_thread_fence(std::memory_order_seq_cst);
    auto res_time = std::chrono::high_resolution_clock::now();
    std::atomic_thread_fence(std::memory_order_seq_cst);
    return res_time;
}

template<class D>
inline long long to_us(const D &d) {
    return std::chrono::duration_cast<std::chrono::microseconds>(d).count();
}

int main() {

    concurrent_que<std::string> qFasta{};
    std::vector <std::vector<size_t>> matrix;

    auto start_time = get_current_time_fenced();

    // read csv

    std::vector <std::string> st;
    read_csv("../data/markers.csv", st);

    std::cout << "Finished reading csv\n";

    // read fasta
    std::thread fastaThread{readAllFasta, &qFasta};

    // build trie
    auto *a = new aho_corasick::AhoCorasick(st);

    // oneapi

    auto start_oneapi = get_current_time_fenced();

    oneapi_find_markers(&qFasta, matrix, a);

    auto end_oneapi = get_current_time_fenced();

    fastaThread.join();

    std::cout << "Execution completed" << std::endl;

    std::cout << "\noneApi time: " << (double) to_us(end_oneapi - start_oneapi) / 1000000.0 << "\n";

    std::cout << "Total time: " << (double) to_us(end_oneapi - start_time) / 1000000.0 << "\n";

    delete (a);

    return 0;
}

