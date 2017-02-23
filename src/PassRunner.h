#ifndef PASSRUNNER_H
#define PASSRUNNER_H

#include "MinCInvocation.h"
#include "Pass.h"

#include <string>
#include <iostream>


class PassRunner {

    unsigned long id_;
    unsigned long counter_;
    std::string directory;
    MinCInvocation invocation_;
    const Pass& pass_;
    bool success_ = false;
    std::string modifiedFile = "";

    long long originalSize = 0;
    long long reducedSize = 0;

    void runTestCommand();


public:
    PassRunner(unsigned long id, unsigned long counter, MinCInvocation& invocation, const Pass& pass)
        : id_(id), counter_(counter), invocation_(invocation), pass_(pass) {
        directory = invocation.tempDir + "/" + createUniqueDirName() + "/";
    }

    void clearDirectory();

    std::string createUniqueDirName() {
        return "minc-j" + std::to_string(id_) + "-" + std::to_string(counter_);
    }

    long bytesReduced() const;

    bool success() const {
        return success_;
    }

    void run();

    void accept();

    static void createRunner(PassRunner* runner) {
        runner->run();
    }
};

#endif // PASSRUNNER_H
