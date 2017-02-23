
#include "MinCInvocation.h"
#include "Pass.h"
#include "PassManager.h"
#include "DirectoryCopier.h"
#include "PassRunner.h"

#include <iostream>
#include <thread>
#include <random>

bool integrityCheck(const MinCInvocation& invocation) {
    if (system(nullptr) == 0) {
        std::cerr << "No shell found on system (system(0) == 0)!" << std::endl;
        exit(2);
    }
    if (invocation.testScript == "") {
        std::cerr << "No testscript given!" << std::endl;
        exit(1);
    }

}

PassRunner* selectOptimum(std::vector<PassRunner>& runners) {
    PassRunner* result = nullptr;
    long biggestReduction = 0;
    for (PassRunner& runner : runners) {
        if (runner.success()) {
            if (runner.bytesReduced() > biggestReduction) {
                result = &runner;
                biggestReduction = runner.bytesReduced();
            }
        }
    }
    return result;
}

int main() {

    unsigned jobs = 4;

    PassManager manager;
    manager.loadPasses("/home/teemperor/min-c/build-min-c-Desktop-Debug");
    for (const Pass& pass : manager.getPasses()) {
        pass.check("DDD");
    }

    DirectoryCopier copier(".");

    MinCInvocation invocation;
    invocation.tempDir = "/home/teemperor/min-c/tmp/";
    invocation.mainDir = &copier;
    invocation.testScript = "/home/teemperor/min-c/test.sh";

    integrityCheck(invocation);

    std::vector<std::thread> threads;
    threads.resize(jobs);


    unsigned long counter = 0;

    int failedReductionIterations = 0;
    const int failedIterationLimit = 100;

    while (true) {
        std::vector<PassRunner> runners;
        for (unsigned long id = 0; id < jobs; id++) {
            const Pass& pass = manager.getRandomPass();
            PassRunner runner(id, counter, invocation, pass);
            runners.push_back(runner);

            std::cout << "[STATUS] Running pass " << pass.name() << " in " << runner.createUniqueDirName() << std::endl;
            counter++;
        }

        for (unsigned long id = 0; id < jobs; id++) {
            PassRunner* runner = &runners[id];
            std::thread t(PassRunner::createRunner, runner);
            threads[id] = std::move(t);
        }

        for (unsigned id = 0; id < jobs; id++) {
            if (threads[id].joinable())
                threads[id].join();
        }

        int successes = 0;


        for (PassRunner& runner : runners) {
            if (runner.success()) {
                successes++;
                std::cout << "[STATUS] Pass " << runner.createUniqueDirName() << " reduced by " << runner.bytesReduced() << std::endl;
            } else {
                std::cout << "[STATUS] Pass " << runner.createUniqueDirName() << " failed" << std::endl;
            }
        }
        std::cout << "Had successes: " << successes << std::endl;
        PassRunner* selectedResult = selectOptimum(runners);
        if (selectedResult) {
            selectedResult->accept();
            failedReductionIterations = 0;
        }
        for (PassRunner& runner : runners) {
            runner.clearDirectory();
        }

        if (selectedResult == nullptr) {
            failedReductionIterations++;
            if (failedReductionIterations >= failedIterationLimit) {
                std::cout << "Couldn't reduce more!" << std::endl;
                return 0;
            }
        }
    }

    //std::string target = "/home/teemperor/min-c/copy";
    //copier.createShallowCopy(target);
    //copier.createDeepCopy(target, copier.getFileList().front());
}
