#include "PassManager.h"

#include <dirent.h>
#include <iostream>
#include <cassert>

PassManager::PassManager()
{

}

PassManager::~PassManager() {
    for (Pass& pass : passes)
        pass.unload();
}

bool hasEnding (std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}
void PassManager::loadPasses(const std::string &directory) {
    DIR *d;
    struct dirent *dir;
    d = opendir(directory.c_str());
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            std::string fileName = dir->d_name;
            std::string fullPath = (directory + "/" + fileName);
            if (hasEnding(fullPath, "Pass.so")) {
                loadPass(fullPath);
            }
        }

        closedir(d);
    } else {
        std::cerr << "PassManager: Couldn't open directory " + directory << std::endl;
    }
}

void PassManager::loadPass(const std::string &soPath) {
    try {
        Pass pass(soPath);
        if (pass.available()) {
          passes.push_back(pass);
          std::cout << "[PassManager] Loaded " << pass.name() << std::endl;
        } else {
          std::cout << "[PassManager] Pass " << pass.name() << " not available on this system." << std::endl;
        }

    } catch (const PassLoadingError& e) {
        std::cerr << "[PassManager] Failed to load pass " << soPath << std::endl;
    }
}


const Pass &PassManager::getRandomPass() {
    assert(!passes.empty());
    auto r = randomGenerator_();
    r %= passes.size();
    return passes.at(r);
}
