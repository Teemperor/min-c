
#include <iostream>

extern "C" {
    int check(const char* path) {
        std::cerr << "CALdddLED PLUGIN " << path << std::endl;
        return 42;
    }

    int transform(const char* path) {

    }
}
