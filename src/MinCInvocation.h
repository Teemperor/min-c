#ifndef MINCINVOCATION_H
#define MINCINVOCATION_H

#include "DirectoryCopier.h"

#include <string>

class MinCInvocation {
public:
    MinCInvocation() {}
    MinCInvocation(int argc, char** argv);
    virtual ~MinCInvocation();

    std::string tempDir = "/tmp";
    std::string testScript = "";
    DirectoryCopier* mainDir = nullptr;

    // unused as of yet
    bool searchtmpfs = false;
    std::string tmpfs = "";
    std::string origSrcDir = "";

    bool logMode = true;

    bool reverifyRuns = true;
};

#endif // MINCINVOCATION_H
