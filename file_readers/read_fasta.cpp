#include <iostream>
#include <fstream>
#include <deque>
#include "../headers/read_fasta.h"
#include <filesystem>
#include <archive.h>
#include <archive_entry.h>

namespace fs = std::filesystem;

void read_fasta(const std::string& file, concurrent_que<std::string>* q) {


    struct archive *a;
    struct archive_entry *entry;
    std::string container;

    a = archive_read_new();
    archive_read_support_filter_all(a);
    archive_read_support_format_raw(a);
    int r = archive_read_open_filename(a, file.c_str(), 10240); // Note 1
    if (r != ARCHIVE_OK)
        exit(1);
    r = 1;

    auto const entry_size = std::size_t{BUFSIZ};
    if (archive_read_next_header(a, &entry) == ARCHIVE_OK)
        archive_entry_set_size(entry, entry_size);

    std::string text = std::string(entry_size, 0);
    while (r != 0) {
        r = archive_read_data(a, &text[0], entry_size);
        if (!text.empty()) {
            container += r != entry_size ? text.substr(0, r) : text;
        }
    }
    archive_free(a);
    for (int i = 0; i < container.size(); ++i){
        if (container[i] == '>'){
            container.erase(i, container.find('\n', i) - i + 1);
        }
    }

    q->push(container);
}

void readAllFasta(concurrent_que<std::string> *q) {
    std::string path = "../data/archive";
    for (auto &entry: fs::recursive_directory_iterator(path)) {
        read_fasta(entry.path(), q);
    }

    q->push(" ");
}