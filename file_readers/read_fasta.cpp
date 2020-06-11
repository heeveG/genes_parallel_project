#include <iostream>
#include <fstream>
#include <deque>
#include "../headers/read_fasta.h"
#include <filesystem>
namespace fs = std::filesystem;

void read_fasta(const std::string& path, concurrent_que<std::string>* q) {


    std::ifstream input(path);

    if (!input.good()) {
        std::cerr << "Error opening '" << "'. Bailing out." << std::endl;
        return;
    }

    std::string container;


    auto const chunk_size = std::size_t{BUFSIZ};


    std::string chunk;
    chunk.resize(chunk_size);
    while (
            input.read(&chunk[0], chunk_size) ||
            input.gcount()
            ) {
        container += input.gcount() != chunk_size ? chunk.substr(0, input.gcount()) : chunk;
    }
    input.close();
    for (int i = 0; i < container.size(); ++i){
        if (container[i] == '>'){
            container.erase(i, container.find('\n', i) - i + 1);
        }
    }

    q->push(container);
}

void readAllFasta(concurrent_que<std::string> *q) {
    std::string path = "../data/archive";
    for (auto &entry: fs::recursive_directory_iterator(path)) {
        read_fasta(entry.path(), q);
    }

    q->push(" ");
}