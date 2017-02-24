#ifndef MINCINVOCATION_H
#define MINCINVOCATION_H

#include "DirectoryCopier.h"

#include <string>

class MinCInvocation {
public:
    MinCInvocation() {}
    MinCInvocation(int argc, char** argv);

    std::string tempDir = "/tmp";
    std::string testScript = "";
    DirectoryCopier* mainDir = nullptr;
};

#endif // MINCINVOCATION_H
