#include "../includes/app.h"

//#define BOOST_ASIO_SEPARATE_COMPILATION

std::unordered_map<int, std::string> *getMarker(std::unordered_map<int, std::string> &markers_map) {
//    const std::string path = "../data/markers.csv";
    std::cout << CSV_DIR << std::endl;
    const std::string path = CSV_DIR;
    std::ifstream markers_file;
    markers_file.open(path, std::ios_base::in);

    if(!markers_file.is_open()){
        std::cerr << "Error opening input markers file" << std::endl;
        return &markers_map;
    }
    int marker_num;
    std::string line;

    int k = 0;
    while (markers_file >> line && k <= 6) {
        k++;
        std::vector<std::string> results;

        boost::split(results, line, boost::is_any_of(","));
        marker_num = stoi(results[0]);
        markers_map.insert({marker_num, results[1]});
        std::cout << markers_map[marker_num] << std::endl;
        std::cout << marker_num << std::endl;
    }
    markers_file.close();
    return &markers_map;
}


int main(int argc, char* argv[]) {
    std::unordered_map<int, std::string> markers_map;
    getMarker(markers_map);

    std::string text = "ihejshershis";

    AhoCorasick a(markers_map);
    std::unordered_map<std::string, std::set<int>> matched = a.matchWords(text);


    for (auto &itr: matched) {
        std::cout << itr.first << ' ';
        for (int it : itr.second)
            std::cout << ' ' << it;
        std::cout << std::endl;
    }


    return 0;
}