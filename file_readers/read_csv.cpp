#include "../headers/read_csv.h"
#include <iostream>
#include<fstream>
#include <boost/algorithm/string.hpp>




void split(std::vector<std::string> &results, std::string &original, char separator) {
    std::string::const_iterator start = original.begin();
    std::string::const_iterator end = original.end();
    std::string::const_iterator next = std::find(start, end, separator);
    std::string str;
    while (next != end) {
        str = std::string(start, next);
        if (str.find(',') !=  std::string::npos)
            results.emplace_back(str);
        start = next + 1;
        next = std::find(start, end, separator);
    }
    str = std::string(start, next);
    if (str.find(',') !=  std::string::npos)
        results.emplace_back(str);

}


void read_csv(std::string path, std::vector<std::string> &markers) {
    std::ifstream fin(path);
    std::string line;
    unsigned long maxMarker = 0;
    int limit = 1000000;
    // Open an existing file

    if (!fin.good()) {
        std::cerr << "Error opening '" << "'. Bailing out." << std::endl;
        exit(-1);
    }

    auto const chunk_size = std::size_t{BUFSIZ};

    std::string str;

    std::string chunk;
    chunk.resize(chunk_size);

    while (
            fin.read(&chunk[0], chunk_size) ||
            fin.gcount()
            ) {
        str += fin.gcount() != chunk_size ? chunk.substr(0, fin.gcount()) : chunk;

    }

    fin.close();
    split(markers, str, '\n');
//    markers.resize(3340138);
    markers.resize(200000);
}



