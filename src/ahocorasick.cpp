//
// Created by heeve on 27.03.20.
//

#include <unordered_map>
#include <set>
#include <queue>
#include "../headers/ahocorasick.h"
#include <cmath>
#include <iostream>
#include <zconf.h>
#include <string>

class Node {

    char value;
    std::set<std::string> retVals;
    std::unordered_map<char, Node *> children;

    Node *fail;
public:

    Node(char init_val)
            : value(init_val), fail(nullptr), retVals(), children() {
    };

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


    std::set<std::string> *getRetVals() {
        return &retVals;
    }

    void addReturnValue(const std::string &added) {
        retVals.insert(added);
    }

    Node *addChild(const char &letter) {
        auto added = childAt(letter);
        if (added == nullptr) {
            added = new Node(letter);
            children[letter] = added;

        }
        return added;


    }

    void setFail(Node *failNode) {
        fail = failNode;
        for (auto &s: *failNode->getRetVals()) {
            addReturnValue(s);
        }
    }

    Node *childAt(const char &letter) {
        auto itr = (children).find(letter);
        if (itr != children.end()) {
            return children.at(letter);
        }
        return nullptr;
    }

};

class AhoCorasick {

    Node *start = new Node('&');
    std::vector<std::string> markers;
    unsigned long longestMarker = 0;
public:

    AhoCorasick() {
        start->setFail(start);
    }

    explicit AhoCorasick(std::vector<std::string> &markersInit) :
            markers(markersInit) {
        start->setFail(start);

        setUpTrie();
    }

    ~AhoCorasick() {
        delete (start);
    }

    void addMarker(std::string marker) {

        if (marker.empty())
            return;

        Node *node = start;
        for (const auto &ch: marker) {
            node = node->addChild(ch);
        }
        node->addReturnValue(marker);


    }

    void setUpTrie() {
        // Add nodes to the trie
        std::string cut;
        for (auto &marker: markers) {
            longestMarker = std::max(longestMarker, marker.size());
            addMarker(marker);
        }
        addFails();
    }

    void addFails() {

        // Add start as fail to each child node of start
        std::queue<Node *> q;
        std::unordered_map<char, Node *> *children = start->getChildren();
        for (auto &itr : *children) {
            itr.second->setFail(start);
            q.push(itr.second);
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
    matchWords(std::string text, int startInx, int endInx, std::unordered_map<std::string, std::set<int>> &matched) {

        Node *node = start;
        Node *child;
        for (int i = startInx; i < endInx; ++i) {
            child = node->childAt(text[i]);
            if (child != nullptr) {
                node = child;
                // ??? Don't know if it's a good practice
                for (auto &returns: *node->getRetVals()) {
                    matched[returns].insert(i - returns.size() + 1);
                }
            } else {
                if (node != start) {

                    node = node->getFail();
                    --i;
                }
            }
        }
    }


    Node *getStart() {
        return start;
    }

    int getMaxMarker() {
        return longestMarker;
    }

};















