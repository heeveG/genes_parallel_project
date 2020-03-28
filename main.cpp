#include <iostream>
#include <unordered_map>
#include <set>
#include "ahocorasick.cpp"

int main() {

    std::set<std::string> strings = {"he", "she", "hers", "his"};
    std::string text = "ihejshershis";

    AhoCorasick a(strings);
    std::unordered_map<std::string, std::set<int>> matched = a.matchWords(text);


    for (auto &itr: matched) {
        std::cout << itr.first << ' ';
        for (int it : itr.second)
            std::cout << ' ' << it;
        std::cout << std::endl;
    }


    return 0;
}