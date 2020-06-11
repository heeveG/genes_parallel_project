//
// Created by heeve on 11.06.20.
//

#ifndef GENOME_ONEAPI_PARALLEL_H
#define GENOME_ONEAPI_PARALLEL_H

#include "concurrent_q.h"
#include "../ahocorasick/ahocorasick.cpp"

void oneapi_find_markers(concurrent_que<std::string> *qFasta, std::vector <std::vector<size_t>> &matrix, aho_corasick::AhoCorasick *a);

#endif //GENOME_ONEAPI_PARALLEL_H
