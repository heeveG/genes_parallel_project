#include "../includes/app.h"
#include <typeinfo>

//#define BOOST_ASIO_SEPARATE_COMPILATION

std::unordered_map<int, std::string> *getMarker(std::unordered_map<int, std::string> &markers_map) {
    int MARKERS_LIMIT = 7;
    // read csv
    const std::string path = CSV_DIR;
    std::ifstream markers_file;
    markers_file.open(path, std::ios_base::in);

    if(!markers_file.is_open()){
        std::cerr << "Error opening input markers file" << std::endl;
        return &markers_map;
    }
    int marker_num;
    std::string line;

    int num_of_markers = 0;
    while (markers_file >> line && num_of_markers <= MARKERS_LIMIT) {
        num_of_markers++;
        std::vector<std::string> results;

        boost::split(results, line, boost::is_any_of(","));
        marker_num = stoi(results[0]);
        markers_map.insert({marker_num, results[1]});
//        std::cout << markers_map[marker_num] << std::endl;
//        std::cout << marker_num << std::endl;
    }
    markers_file.close();
    return &markers_map;
}


int main(int argc, char* argv[]) {
    // TODO: get configs from the file
    // read fasta
    FastqReader reader("../data/archive/pseudo88.fasta.gz");
    Read* r1 = NULL;

    int counter = 0;
    while(true) {
        if (counter == 1) break;
        r1=reader.read();
        if(r1 == NULL) {
//            std::cout << "read nothing" <<  counter << std::endl; // xxx
            break;
        }
//        xxx (3)
//        if (r1->mSeq.mStr.length() != 0) {
//            std::cout << "This is reading # " << counter << std::endl;
//        }
//        delete r1; // xxx
        counter++;
    }
    std::cout << "Name ::: " << r1->mName << std::endl;

    // read markers
    std::unordered_map<int, std::string> markers_map;
    getMarker(markers_map);

    std::cout << "Sequence ::: " << std::endl;

    // run algorithm
    AhoCorasick a(markers_map);
    std::unordered_map<std::string, std::set<int>> matched = a.matchWords(r1->mSeq.mStr);

    for (auto &itr: matched) {
        std::cout << itr.first << ' ';
        for (int it : itr.second)
            std::cout << ' ' << it;
        std::cout << std::endl;
    }

    std::cout << "Strand ::: " << r1->mStrand << std::endl;
    for (int i = 0; i < 15; ++i)
        std::cout << (r1->mSeq.mStr)[8106744 + i] << " ";

    return 0;
}