#ifndef PASSRUNNER_H
#define PASSRUNNER_H

#include "MinCInvocation.h"
#include "Pass.h"
#include "TestDirectory.h"

#include <string>
#include <iostream>
#include <json.hpp>

class PassManager;

class PassRunner {

    unsigned long id_;
    unsigned long counter_;
    std::string directory;
    MinCInvocation invocation_;
    Pass* pass_ = nullptr;
    bool success_ = false;
    std::string modifiedFile = "";

    bool changedStructure_ = false;
    long long originalSize = 0;
    long long reducedSize = 0;

    TestDirectory testDirectory_;

    void runTestCommand();


public:
    PassRunner(unsigned long id, unsigned long counter, MinCInvocation& invocation, Pass& pass);

    nlohmann::json toJSON();

    void updateFromJSON(nlohmann::json& json, PassManager& manager);

    void clearDirectory();

    std::string createUniqueDirName() {
        return "minc-j" + std::to_string(id_) + "-" + std::to_string(counter_);
    }

    long bytesReduced() const;

    bool success() const {
        return success_;
    }

    bool changedStructure() const {
        return changedStructure_;
    }

    Pass& getPass() {
      return *pass_;
    }

    void markAsFailed() {
      success_ = false;
    }

    void run();

    void accept();

    static void createRunner(PassRunner* runner) {
        runner->run();
    }
};

#endif // PASSRUNNER_H
