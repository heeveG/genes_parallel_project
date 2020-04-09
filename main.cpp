#include <iostream>
#include <set>
#include "ahocorasick.cpp"
#include <thread>
#include <vector>



int main() {
    int numThreads = 10;

    std::set<std::string> strings = {"he", "she", "hers", "his"};
    std::string text = "ihejshershisblahheshehers";

    auto *a = new AhoCorasick(strings);

    std::vector<myMap> maps(numThreads);
    std::vector<std::thread> threads;


    for (int i = 0; i < numThreads; ++i){
        threads.emplace_back(&AhoCorasick::matchWords, a, text, std::ref(maps[i]));
    }

    for (auto& t: threads){
        t.join();
    }

    for (auto &mp: maps) {
        for (auto &itr: mp) {
            std::cout << itr.first << ' ';
            for (int it : itr.second)
                std::cout << ' ' << it;
            std::cout << std::endl;
        }
        std::cout << std::endl;

    }

    delete(a);


    return 0;
}