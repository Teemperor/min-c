
#include "Pass.h"
#include "PassManager.h"
#include <iostream>
#include "DirectoryCopier.h"

class Runner {

    unsigned id_;
    const std::string directory;

public:
    Runner(unsigned id) : id_(id) {
    }
};

int main() {
    PassManager manager;
    manager.loadPasses("/home/teemperor/min-c/build-min-c-Desktop-Debug");

    DirectoryCopier copier(".");

    std::string target = "/home/teemperor/min-c/copy";
    copier.createShallowCopy(target);
    copier.createDeepCopy(target, copier.getFileList().front());
}
