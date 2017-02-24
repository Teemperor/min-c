#ifndef GENERICFILTER_H
#define GENERICFILTER_H

#include <regex>
#include <string>

bool hasEnding(std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

// returns 1 for filepaths that we usually should reduce
int commonFilter(const std::string& path) {
    if (hasEnding(path, ".modulemap"))
        return 0;

    if (std::regex_search(path, std::regex{"clang-\\d.\\d$"})) {
        return 0;
    }
    if (std::regex_search(path, std::regex{"so.$"})) {
        return 0;
    }
    return 1;
}

#endif // GENERICFILTER_H
