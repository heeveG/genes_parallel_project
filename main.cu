#include <iostream>
#include <set>
#include "headers/read_csv.h"
#include "headers/read_fasta.h"
#include <thread>
#include <vector>
#include <chrono>
#include <atomic>
#include <string.h>
#include <cuda_profiler_api.h>

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


void readAllFasta(concurrent_que<std::string> *q) {
    auto start = get_current_time_fenced();
    std::string path = "../data/archive/pseudo88.fasta";
//    for (const auto &entry: fs::directory_iterator(path)) {
    read_fasta(path, q);
//    }
    q->push(" ");
    auto end = get_current_time_fenced();
}

__global__
void matchWords(const char *str, size_t *matched, aho_corasick::Node *dVec, int size) {

    aho_corasick::Node *node;
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;
    for (int iter = i; iter < size; iter += stride) {
        node = &dVec[0];
        while (i < size) {
            char letter = str[i];
            if (letter == 'N') {
                break;
            }

            int nodesIndex = node->children[abs((letter - 65) / 2 % 5 - 1)];
            if (nodesIndex == 0) {
                break;
            }

            node = &dVec[nodesIndex];

            if (node->markerHash) {
                matched[node->markerId] = node->markerHash;
            }
            ++i;
        }

        i = iter + stride;
    }
}


int main() {

    concurrent_que<std::string> qFasta{};

    // read csv
    auto start_t = get_current_time_fenced();

    std::vector<std::string> st;
    auto markers_start = get_current_time_fenced();
    read_csv("../data/markers.csv", st);
    auto markers_end = get_current_time_fenced();
    std::cout << "Finished reading csv\n";


    // read fasta
    auto readFastaStart = get_current_time_fenced();

    std::thread fastaThread{readAllFasta, &qFasta};
    fastaThread.join();

    auto readFastaEnd = get_current_time_fenced();

    std::cout << "Fasta read time : " << to_us(readFastaEnd - readFastaStart) << std::endl;

    auto c1 = get_current_time_fenced();
    auto *a = new aho_corasick::AhoCorasick(st);
    auto c2 = get_current_time_fenced();

    std::cout << "Build trie time : " << to_us(c2 - c1) << std::endl;

    std::cout << "Size of nodes in bytes: " << a->nodeNum * sizeof(aho_corasick::Node) << "\n";

    std::string fasta = qFasta.pop();

    std::cout << fasta.size() << " fasta size\n";
    char *cudaStr;
    size_t *cudaMatched;
    aho_corasick::Node *devNodes;

    auto startCopying = get_current_time_fenced();
    if (cudaMalloc((void **) &devNodes, a->nodeNum * sizeof(aho_corasick::Node)) == cudaErrorMemoryAllocation) {
        exit(1);
    }
    cudaMemcpy(devNodes, a->nodes, a->nodeNum * sizeof(aho_corasick::Node), cudaMemcpyHostToDevice);


    int blockSize = 256;
    int numBlocks = ((int) fasta.size() + blockSize - 1) / blockSize;


    auto startMatching = get_current_time_fenced();

    cudaMallocManaged((void **) &cudaStr, fasta.size());
    cudaMallocManaged((void **) &cudaMatched, st.size() * sizeof(size_t));
    cudaMemcpy(cudaStr, fasta.data(), fasta.size(), cudaMemcpyHostToDevice);
    cudaMemset(cudaMatched, 0, st.size() * sizeof(size_t));

    matchWords << < numBlocks, blockSize >> > (cudaStr, cudaMatched, devNodes, fasta.size());

    cudaDeviceSynchronize();


    auto *matches = (size_t *) malloc(st.size() * sizeof(size_t));
    if (matches == nullptr) {
        std::cout << "Allocation failed\n";
    }
    cudaMemcpy(matches, cudaMatched, st.size() * sizeof(size_t), cudaMemcpyDeviceToHost);

    auto endMatching = get_current_time_fenced();

    int count = 0;
    for (int i = 0; i < st.size(); ++i) {
        if (matches[i] != 0) {
            count += a->markerIdMap[matches[i]].size();
        }
    }
    auto end_t = get_current_time_fenced();

    std::cout << "count : " << count << "\n";

    std::cout << to_us(startMatching - startCopying) << " time to copy\n";


    std::cout << "Markers: " << st.size() << "\n";
    std::cout << "Trie build time: " << (double) to_us(c2 - c1) / 1000000.0 << "\n";
    std::cout << "Mathcing time: " << (double) to_us(endMatching - startMatching) / 1000000.0 << "\n";
    std::cout << "Total time: " << (double) to_us(end_t - start_t) / 1000000.0 << "\n";
    std::cout << "Read markers.csv time: " << (double) to_us(markers_end - markers_start) / 1000000.0 << "\n";


    delete (a);
    delete (matches);
    cudaFree(devNodes);
    cudaFree(cudaStr);
    cudaFree(cudaMatched);
    cudaProfilerStop();
    return 0;
}

