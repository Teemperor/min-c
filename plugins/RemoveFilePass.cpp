
#include "GenericFilter.h"

#include <unistd.h>
#include <iostream>
#include <cassert>

extern "C" {
    int check(const char* path) {
        return commonFilter(path);
    }

    int transform(const char* path, unsigned long random) {
        auto result = unlink(path);
        assert(result != -1);
        return 1;
    }
}
