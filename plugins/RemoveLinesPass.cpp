
#include "GenericFilter.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

extern "C" {
    int available() {
      return 1;
    }

    int check(const char* path) {
        return commonFilter(path);
    }

    int transform(const char* path, unsigned long random) {
        std::ifstream myfile(path);
        std::string line;
        std::vector<std::string> lines;
        while (std::getline(myfile, line))
        {
           lines.push_back(line);
        }
        if (lines.empty())
            return 0;

        size_t indexToSkip = random % lines.size();
        std::ofstream t(path);
        for (size_t i = 0; i < lines.size(); ++i) {
            if (i != indexToSkip) {
                t << lines[i] << '\n';
            }
        }

        return 1;
    }
}
