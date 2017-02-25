#ifndef PASSMANAGER_H
#define PASSMANAGER_H

#include "Pass.h"

#include <string>
#include <vector>
#include <random>

class NoPassWithNameException {};

class PassManager {
    std::vector<Pass> passes;
    std::mt19937 randomGenerator_;

public:
    PassManager();
    virtual ~PassManager();

    void loadPasses(const std::string& directory);

    void loadPass(const std::string& soPath);

    size_t passCount() const {
      return passes.size();
    }

    const std::vector<Pass>& getPasses() const {
        return passes;
    }

    Pass &getPass(const std::string& name);

    const Pass &getTopPass() const;

    Pass &getRandomPass();
};

#endif // PASSMANAGER_H
