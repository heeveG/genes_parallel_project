//
// Created by heeve on 27.03.20.
//

#include <unordered_map>
#include <set>
#include <queue>
#include <string>
#include <thrust/device_vector.h>

namespace aho_corasick {
    class Node {

    public:
        int children[4];
        size_t markerHash = 0;
        int markerId;

        Node() {
            memset(children, 0, 4 * sizeof(int));
        };


        void addReturnValue(size_t hash, int id) {
            markerHash = hash;
            markerId = id;
        }

        int childIndex(char letter) {
            return abs(letter % 65 / 2 % 5 - 1);
        }

        int addChild(const char letter, int &size) {
            auto childIndx = childAt(letter);
            if (childIndx == 0) {
                int index = childIndex(letter);
                children[index] = size;
                return size;
            }
            return childIndx;
        }


        int childAt(const char &letter) {
            int index = childIndex(letter);
            int childIndx = children[index];
            if (index != -1 && childIndx != 0) {
                return childIndx;
            }
            return 0;
        }


    };

    class AhoCorasick {

    public:
        std::vector<std::string> markers;
        int nodeNum = 0;
        int capacity = 1;
        Node *nodes = new Node[capacity];

        std::unordered_map<size_t, std::set<int>> markerIdMap;

        AhoCorasick() {
        }

        explicit AhoCorasick(std::vector<std::string> &markersInit) :
                markers(markersInit) {

            setUpTrie();

        }

        ~AhoCorasick() {
            delete[] (nodes);
        }


        void setUpTrie() {
            reserve(2 * markers.size());
            addNode();
            std::hash<std::string> hash_fn;
//            // Add nodes to the trie
            for (auto &markerItem: markers) {
                if (markerItem.empty())
                    continue;
                int markerId;

                markerId = std::stoi(markerItem.substr(0, markerItem.find(',')));

                std::string marker = markerItem.substr(markerItem.find(',') + 1);

                if (marker.empty())
                    continue;

                int next = 0;

                size_t hash = hash_fn(marker);
                if (markerIdMap.find(hash) == markerIdMap.end()) {
                    for (const auto &ch: marker) {
                        next = nodes[next].addChild(ch, nodeNum);
                        if (next == nodeNum) {
                            addNode();
                        }
                    }
                    nodes[next].addReturnValue(hash, markerId);
                }

                markerIdMap[hash].insert(markerId);
            }
        };

        void reserve(int num) {
            if (capacity < num) {
                Node *newNodes = new Node[num + 1];
                std::copy(nodes, nodes + nodeNum + 1, newNodes);
                delete[]nodes;
                nodes = newNodes;
                capacity = num + 1;
            }

        }

        void addNode() {
            Node *newNode = new Node();
            if (nodeNum == capacity) {
                reserve(2 * capacity);
            }

            nodes[nodeNum] = *newNode;
            ++nodeNum;
        }
    };

}
