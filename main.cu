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
    std::string path = "data/archive/pseudo88.fasta";
//    for (const auto &entry: fs::directory_iterator(path)) {
    read_fasta(path, q);
//    }
    q->push(" ");
    auto end = get_current_time_fenced();
}

__global__
void matchWords(const char *str, int *matched, aho_corasick::Node *dVec[], int size) {
    aho_corasick::Node *node = dVec[0];
//    printf("TEST: %d\n", dVec[5]->retsCount);
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;
    int index;
    for (int iter = i; iter < size; iter += stride) {
        node = dVec[0];
        while (i < size) {
            char letter = str[i];
            switch (letter) {

                case 'A':
                    index = 0;
                    break;
                case 'C':
                    index = 1;
                    break;
                case 'G':
                    index = 2;
                    break;
                case 'T':
                    index = 3;
                    break;
                default:
                    index = -1;

            }
            if (index == -1)
                break;
            int nodesIndex = node->cudaChildren[index];
            if (nodesIndex == 0) {
                break;
            }
            node = dVec[nodesIndex];

            for (int j = 0; j < node->retsCount; ++j) {
                matched[node->cudaRets[j]] = 1;
            }
            ++i;
        }
        i = iter + stride;
    }
}

int main() {

    concurrent_que<std::string> qFasta{};

    // read csv

    std::vector<std::string> st;
    read_csv("data/markers.csv", st);

    std::cout << "Finished reading csv\n";


    // read fasta

    std::thread fastaThread{readAllFasta, &qFasta};
    fastaThread.join();


    auto *a = new aho_corasick::AhoCorasick(st);
    std::cout << "Built trie\n";



    std::string fasta = qFasta.pop();
    char *cudaStr;
    int *cudaMatched;

    auto startCopying = get_current_time_fenced();

    cudaMallocManaged(&cudaStr, fasta.size());
    cudaMallocManaged(&cudaMatched, st.size() * sizeof(int));


    cudaMemcpy(cudaStr, fasta.data(), fasta.size(), cudaMemcpyHostToDevice);
    cudaMemset(cudaMatched, 0, st.size() * sizeof(int));

    auto **nodesPtr = (aho_corasick::Node **) malloc(a->nodes.size() * sizeof(aho_corasick::Node *));

    int i = 0;

    for (auto &node: a->nodes) {

        auto *newNode = new aho_corasick::Node(' ');

        cudaMalloc((void **) &(newNode->cudaChildren), sizeof(int) * node->children.size());
        cudaMemcpy(newNode->cudaChildren, node->children.data(), sizeof(int) * node->children.size(),
                   cudaMemcpyHostToDevice);
        cudaMalloc((void **) &(newNode->cudaRets), sizeof(int) * node->retVals.size());
        cudaMemcpy(newNode->cudaRets, node->retVals.data(), sizeof(int) * node->retVals.size(),
                   cudaMemcpyHostToDevice);

        newNode->retsCount = node->retVals.size();

        aho_corasick::Node *devNode;

        cudaMalloc((void **) &devNode, sizeof(aho_corasick::Node));
        cudaMemcpy(devNode, newNode, sizeof(aho_corasick::Node), cudaMemcpyHostToDevice);

        nodesPtr[i++] = devNode;
    }


    aho_corasick::Node **devNodes;

    cudaMalloc((void ***) &devNodes, a->nodes.size() * sizeof(aho_corasick::Node *));
    cudaMemcpy(devNodes, nodesPtr, a->nodes.size() * sizeof(aho_corasick::Node *), cudaMemcpyHostToDevice);

    int blockSize = 256;
    int numBlocks = ((int) fasta.size() + blockSize - 1) / blockSize;


    std::cout << "Copied \n";

    auto startMatching = get_current_time_fenced();

    matchWords << < numBlocks, blockSize >> > (cudaStr, cudaMatched, devNodes, fasta.size());

    cudaDeviceSynchronize();

    auto endMatching = get_current_time_fenced();


    int matches[st.size()];
    cudaMemcpy(&matches[0], cudaMatched, st.size() * sizeof(int), cudaMemcpyDeviceToHost);



//    for (int i = 0; i < 6000; ++i) {
//        if (matches[i] == 1) {
//            std::cout << matches[i] << " at " << i << "\n";
//        }
//
//    }

    std::cout << to_us(startMatching - startCopying) << " time to copy\n";

    std::cout << to_us(endMatching - startMatching) << " time to match\n";


    delete (a);
    delete (nodesPtr);
    cudaFree(devNodes);
    cudaFree(cudaStr);
    cudaFree(cudaMatched);
    cudaProfilerStop();
    return 0;
}

