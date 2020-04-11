#include <iostream>
#include <set>
#include "headers/read_csv.h"
#include "headers/read_fasta.h"
#include <thread>
#include <vector>
#include <chrono>
#include <atomic>
#include <filesystem>

namespace fs = std::filesystem;

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

// parallel read

//void readAllFasta(concurrent_que<std::string>* q){
//    std::string path = "../data/archive";
//    std::vector<std::thread> threads;
//    for (const auto & entry: fs::directory_iterator(path))
//        threads.emplace_back(read_fasta, entry.path(), q);
//
//    for (auto& t: threads)
//        t.join();
//
//}


void readAllFasta(concurrent_que<std::string> *q) {
    auto start = get_current_time_fenced();
    std::string path = "../data/archive";
    for (const auto &entry: fs::directory_iterator(path)) {
        read_fasta(entry.path(), q);
    }
    q->push(" ");
    auto end = get_current_time_fenced();
}


int main() {
    concurrent_que<std::string> qFasta{};
    int numThreads = 8;
    int numFastas = 3;

    auto start_time = get_current_time_fenced();

    // read csv

    std::vector<std::string> st;
    read_csv("../data/markers.csv", st);

    std::cout << "Finished reading csv\n";

    auto read_csv = get_current_time_fenced();

    // read fasta

    std::thread fastaThread{readAllFasta, &qFasta};

    auto bld_st = get_current_time_fenced();
    auto *a = new AhoCorasick(st);
    auto builded_nomy = get_current_time_fenced();


    std::vector<std::vector<myMap>> allMaps(numFastas);
    std::vector<std::thread> threads;

    int start = 0, end = 0, maxMarker = a->getMaxMarker(), sizeFasta = 0, iter = 0;
    std::string text;
    auto start_thread = get_current_time_fenced();

    std::vector<myMap> mapPerFasta;


    while (qFasta.back() != " ") {
        threads.clear();
        mapPerFasta = allMaps[iter];
        mapPerFasta.resize(numThreads);
        text = qFasta.pop();
        sizeFasta = text.size();
        for (int i = 0; i < numThreads; ++i) {
            start = (int) (i * (sizeFasta / numThreads)) - maxMarker;
            end = (int) ((i + 1) * (sizeFasta / numThreads)) + maxMarker;
            threads.emplace_back(&AhoCorasick::matchWords, a, text, start > 0 ? start : 0,
                                 end < sizeFasta ? end : sizeFasta, std::ref(mapPerFasta[i]));
        }
        for (auto &t: threads) {
            t.join();
        }
        ++iter;
    }
    auto bld_end = get_current_time_fenced();
    fastaThread.join();




    std::cout << to_us(builded_nomy - bld_st) << std::endl;
    std::cout << to_us(bld_end - builded_nomy) << std::endl;



    auto end_thread = get_current_time_fenced();

    delete (a);


    std::cout << "\nCSV time : " << to_us(read_csv - start_time)
              << "\nBuilding trie time : " << to_us(read_csv - read_csv) <<
              "\nCalculate : " << to_us(end_thread - start_thread);


    std::cout << "\n Total: " << to_us(end_thread - start_time);


    return 0;
}

