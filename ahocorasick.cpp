//
// Created by heeve on 27.03.20.
//

#include <unordered_map>
#include <set>
#include <queue>
#include "ahocorasick.h"


class Node {

    char value;
    std::set<std::string> retVals;
    std::unordered_map<char, Node *> children;

    Node *fail;
public:

    Node(char init_val)
            : value(init_val) {};

    ~Node() {
        if (!children.empty()) {
            for (auto &itr: children) {
                delete (itr.second);
            }
        }
    }

    Node *getFail() {
        return fail;
    }

    std::unordered_map<char, Node *> *getChildren() {
        return &children;
    }


    std::set<std::string> &getRetVals() {
        return retVals;
    }

    void addReturnValue(const std::string &added) {
        retVals.insert(added);
    }

    void addChild(const char &letter) {
        children.insert({letter, new Node(letter)});
    }

    void setFail(Node *failNode) {
        fail = failNode;
        for (auto &s: failNode->getRetVals()) {
            addReturnValue(s);
        }
    }

    Node *childAt(const char &letter) {
        auto itr = children.find(letter);
        if (itr != children.end()) {
            return children.at(letter);
        }
        return nullptr;
    }

};

class AhoCorasick {

    Node *start = new Node('&');
    std::set<std::string> markers;
public:

    AhoCorasick(std::set<std::string> &markersInit) :
            markers(markersInit) {
        start->setFail(start);
        setUpTrie();
    }

    ~AhoCorasick() {
        delete (start);
    }

    void setUpTrie() {
        // Add nodes to the trie
        Node *topNode;
        Node *node;

        for (auto &marker: markers) {
            topNode = start;
            for (int i = 0; i < marker.size(); ++i) {
                node = topNode->childAt(marker[i]);
                if (node == nullptr) {
                    node = topNode;
                    node->addChild(marker[i]);
                    topNode = node->childAt(marker[i]);
                } else {
                    topNode = node;
                }
                if (i == marker.size() - 1) {
                    topNode->addReturnValue(marker);
                }
            }
        }
        addFails();
    }

    void addFails() {

        // Add start as fail to each child node of start
        std::queue<Node *> q;
        std::unordered_map<char, Node *> *children = start->getChildren();
        for (auto &itr : *children) {
            q.push(itr.second);
            itr.second->setFail(start);
        }

        Node *parent;
        Node *fail;
        Node *childFail;

        // Add fails to each other node
        while (!q.empty()) {
            parent = q.front();
            children = parent->getChildren();

            for (auto &itr: *children) {
                q.push(itr.second);
                fail = parent;

                do {
                    fail = fail->getFail();
                    childFail = fail->childAt(itr.first);
                } while (childFail == nullptr && fail != start);

                if (childFail == nullptr) {
                    childFail = start;
                }

                itr.second->setFail(childFail);
            }
            q.pop();

        }


    }


    void
    matchWords(std::string text, int startInx, int endInx,  std::unordered_map<std::string, std::set<int>> &matched) {

        std::unordered_map<std::string, std::set<int>> output;
        Node *node = start;
        Node *child;
        for (int i = startInx; i < endInx; ++i) {
            child = node->childAt(text[i]);
            if (child != nullptr) {
                node = child;
                // ??? Don't know if it's a good practice
                for (auto &returns: node->getRetVals()) {
                    output[returns].insert(i - returns.size() + 1 );
                }
            } else {
                if (node != start) {

                    node = node->getFail();
                    --i;
                }
            }
        }
        matched = output;
    }


    Node *getStart() {
        return start;
    }

};















