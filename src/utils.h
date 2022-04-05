#ifndef CAPSTONE_UTILS_H
#define CAPSTONE_UTILS_H

// #define WINDOWS  /* uncomment this line to use it for windows.*/
#ifdef WINDOWS
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif
#include<iostream>

using namespace std;

string GetCurrentBuildDir(void) {
    char buff[FILENAME_MAX];
    GetCurrentDir( buff, FILENAME_MAX );
    string current_build_dir(buff);
    return current_build_dir;
}

string GetPreviousPathDir(string path) {
    size_t botDirPos = path.find_last_of("/");
    string dir = path.substr(0, botDirPos);

    return dir;
}

string concatenatePaths(string base_path, string file_name){
    stringstream ss;

    if ((base_path.find_last_of("/") == base_path.length()) && (file_name.find_first_of("/") != 0)) {
        ss << base_path << file_name;
    }
    else if ((base_path.find_last_of("/") != base_path.length()) && (file_name.find_first_of("/") == 0)){
        ss << base_path << file_name;
    }
    else if ((base_path.find_last_of("/") != base_path.length()) && (file_name.find_first_of("/") != 0)) {
        ss << base_path << "/" << file_name;
    }
    else {
        base_path.pop_back();
        ss << base_path << file_name;
    }



    return ss.str();
}

// produce chunks from
//https://stackoverflow.com/questions/14226952/partitioning-batch-chunk-a-container-into-equal-sized-pieces-using-std-algorithm
template < typename Iterator >
void for_each_interval(Iterator begin, Iterator end, size_t interval_size, std::function<void( Iterator, Iterator )> operation )
{
    auto to = begin;

    while ( to != end )
    {
        auto from = to;

        auto counter = interval_size;
        while ( counter > 0 && to != end )
        {
            ++to;
            --counter;
        }

        operation( from, to );
    }
}

#endif //CAPSTONE_UTILS_H
