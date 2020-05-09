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

        typedef Node *node_ptr;

    public:
        char value;
        int *cudaRets;
        int *cudaChildren;

        int retsCount;
        std::vector<int> retVals;
        std::vector<int> children;


        Node(char init_val)
                : value(init_val), retVals(), retsCount(0) {
            children.resize(4);
            std::fill(children.begin(), children.end(), 0);
        };

//        ~Node() {
//            if (!children.empty()) {
//                for (int i = 0; i < children.size(); ++i) {
//                    if (children[i] != nullptr)
//                        delete (children[i]);
//                }
//            }
//        }


        void addReturnValue(int added) {
            retVals.push_back(added);
        }

        int childIndex(char letter) {
            switch (letter) {
                case 'A':
                    return 0;
                case 'C':
                    return 1;
                case 'G':
                    return 2;
                case 'T':
                    return 3;
                default:
                    return -1;
            }
        }

        node_ptr addChild(const char letter, std::vector<node_ptr> &nodes) {
            auto childIndx = childAt(letter);
            if (childIndx == 0) {
                int index = childIndex(letter);

                assert(index != -1); // childIndex method should always handle all possible inputs

                int newItem = nodes.size();
                node_ptr newNode = new Node(letter);
                nodes.push_back(newNode);
                children[index] = newItem;
                return newNode;
            }
            return nodes[childIndx];
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
        typedef Node *node_ptr;
    public:

        node_ptr start = new Node('&');


        std::vector<std::string> markers;


        std::vector<node_ptr> nodes{1};
        unsigned long longestMarker = 0;

        AhoCorasick() {
        }

        explicit AhoCorasick(std::vector<std::string> &markersInit) :
                markers(markersInit) {

            setUpTrie();
        }

        ~AhoCorasick() {
            delete (start);
        }


        void setUpTrie() {
            nodes.reserve(2 * markers.size());
            nodes[0] = start;
            // Add nodes to the trie
            node_ptr node;
            for (auto &markerItem: markers) {

                int markerId;
                try {
                    markerId = std::stoi(markerItem.substr(0, markerItem.find(',')));
                }
                catch (std::invalid_argument) {
                    continue;
                }
                std::string marker = markerItem.substr(markerItem.find(',') + 1);
                longestMarker = std::max(longestMarker, marker.size());
                if (marker.empty())
                    continue;
                node = start;
                for (const auto &ch: marker) {
                    node = node->addChild(ch, nodes);
                }
                node->addReturnValue(markerId);

            }
        }
    };
};

