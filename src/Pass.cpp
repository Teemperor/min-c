#include "Pass.h"

#include <stdlib.h>
#include <dlfcn.h>
#include <iostream>
#include <algorithm>

Pass::Pass(const std::string &path)
{
    auto lastSlashIndex = path.find_last_of('/');
    if (lastSlashIndex == std::string::npos)
        lastSlashIndex = 0;

    name_ = path.substr(lastSlashIndex);
    char *error;

    handle = dlopen (path.c_str(), RTLD_LAZY);
    if (!handle) {
        std::cerr << "ERROR: " << dlerror() << std::endl;
        throw PassLoadingError();
    }

    checkCall = reinterpret_cast<check_callback>(dlsym(handle, "check"));
    if ((error = dlerror()) != NULL) {
        std::cerr << "ERROR: " << dlerror() << std::endl;
        throw PassLoadingError();
    }

    transformCall = reinterpret_cast<transform_callback>(dlsym(handle, "transform"));
    if ((error = dlerror()) != NULL) {
        std::cerr << "ERROR: " << dlerror() << std::endl;
        throw PassLoadingError();
    }
}

void Pass::unload() {
    dlclose(handle);
}

bool Pass::check(const std::string& path) const {
    return (*checkCall)(path.c_str());
}

bool Pass::transform(const std::string& path, unsigned long random) const {
    return (*transformCall)(path.c_str(), random);
}
