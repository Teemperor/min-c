#ifndef PASSMANAGER_H
#define PASSMANAGER_H

#include <string>
#include <vector>

#include "Pass.h"

class PassManager
{
    std::vector<Pass> passes;

public:
    PassManager();

    void loadPasses(const std::string& directory);

    void loadPass(const std::string& soPath);
};

#endif // PASSMANAGER_H
