
#include "GenericFilter.h"

#include <string>
#include <fstream>
#include <streambuf>


extern "C" {
    int available() {
      if (system("topformflat 1>/dev/null") == 0) {
        return 1; // works
      } else {
        return 0; // doesn't works
      }
    }

    int check(const char* path) {
        return commonFilter(path);
    }

    int transform(const char* path, unsigned long random) {
        const std::string topformflatCommand = "topformflat <'" + std::string(path) + "' >'" + std::string(path) + ".red'";
        if (system(topformflatCommand.c_str()))
            return 0;
        const std::string mvCommand = "mv '" + std::string(path) + ".red' '" + std::string(path) + "'";
        if (system(mvCommand.c_str()))
            return 0;
        return 1;
    }
}
