
#include <string>
#include <fstream>
#include <streambuf>


extern "C" {
    int check(const char* path) {
        return 1;
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