#include <iostream>
#include <set>
#include "ahocorasick.cpp"
#include <thread>
#include <vector>


int main() {
    int numThreads = 25;

    std::set<std::string> strings = {"he", "she", "hers", "his"};
    std::string text = "ihejshershisblahheshehers";
    std::cout << text.size() << std::endl;
    auto *a = new AhoCorasick(strings);

    std::vector<myMap> maps(numThreads);
    std::vector<std::thread> threads;

    int maxMarker = 4;
    int start = 0, end = 0;

    for (int i = 0; i < numThreads; ++i) {
        start = (int) (i * (text.size() / numThreads)) - maxMarker;
        end = (int) ((i + 1) * (text.size() / numThreads)) + maxMarker;
        threads.emplace_back(&AhoCorasick::matchWords, a, text, start > 0 ? start : 0,
                             end < text.size() ? end : text.size(), std::ref(maps[i]));
    }

    for (auto &t: threads) {
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

    delete (a);


    return 0;
}