//
// Created by Sofiia Petryshyn on 08/04/2020.
//

#ifndef READ_ARCHIVE_UTIL_H
#define READ_ARCHIVE_UTIL_H

#include <sys/stat.h>

// check if a string is a directory
inline bool is_directory(const  string& path)
{
    bool isdir = false;
    struct stat status;
    // visual studion use _S_IFDIR instead of S_IFDIR
    // http://msdn.microsoft.com/en-us/library/14h5k7ff.aspx
#ifdef _MSC_VER
#define S_IFDIR _S_IFDIR
#endif
    stat( path.c_str(), &status );
    if ( status.st_mode &  S_IFDIR  ) {
        isdir = true;
    }
// #endif
    return isdir;
}

// Remove invalid sequence characters from a string
inline void str_keep_valid_sequence(  string& s, bool forceUpperCase = false)
{
    size_t total = 0;
    const char case_gap = 'a' - 'A';
    for( size_t it =0; it < s.size(); it++) {
        char c = s[it];
        if(forceUpperCase && c>='a' && c<='z') {
            c -= case_gap;
        }
        if(  isalpha(c) || c == '-' || c == '*' ) {
            s[total] = c;
            total ++;
        }
    }

    s.resize(total);
}

inline bool ends_with( string const & value,  string const & ending)
{
    if (ending.size() > value.size()) return false;
    return  equal(ending.rbegin(), ending.rend(), value.rbegin());
}

inline void error_exit(const string& msg) {
    cerr << "ERROR: " << msg << endl;
    exit(-1);
}

#endif //READ_ARCHIVE_UTIL_H
