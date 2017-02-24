
#include "MinCInvocation.h"
#include "Pass.h"
#include "PassManager.h"
#include "DirectoryCopier.h"
#include "PassRunner.h"
#include "ConsoleStatus.h"

#include <iostream>
#include <thread>
#include <random>
#include <signal.h>

const int failedIterationLimit = 40;

ConsoleStatus consoleStatus;

static int mincShouldExit = false;

void SIGINTHandler(int s){
    mincShouldExit = true;
}

static struct sigaction sigIntHandler;

void startExitListener() {
    sigIntHandler.sa_handler = SIGINTHandler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, NULL);
    sigaction(SIGHUP, &sigIntHandler, NULL);
}

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

    unsigned jobs = 10;

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

    while (true) {
        std::vector<PassRunner> runners;
        for (unsigned long id = 0; id < jobs; id++) {
            const Pass& pass = manager.getRandomPass();
            PassRunner runner(id, counter, invocation, pass);
            runners.push_back(runner);

            //std::cout << "[STATUS] Running pass " << pass.name() << " in " << runner.createUniqueDirName() << "\n";
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
                //std::cout << "[STATUS] Pass " << runner.createUniqueDirName() << " reduced by " << runner.bytesReduced() << "\n";
            } else {
                //std::cout << "[STATUS] Pass " << runner.createUniqueDirName() << " failed\n";
            }
        }
        //std::cout << "[STATUS] Had successes: " << successes << "\n";
        PassRunner* selectedResult = selectOptimum(runners);
        if (selectedResult) {
            consoleStatus.appendMessage("Reducing... ");
            selectedResult->accept();
            failedReductionIterations = 0;
            if (selectedResult->changedStructure()) {
                invocation.mainDir->recreate();
            }
            consoleStatus.addReducedBytes(selectedResult->bytesReduced());
        }
        for (PassRunner& runner : runners) {
            runner.clearDirectory();
        }

        if (mincShouldExit) {
            std::cout << "\nExited due to user input\n";
            return 0;
        }

        if (selectedResult == nullptr) {
            failedReductionIterations++;
            consoleStatus.appendMessage("Stuck at reducing ");
            consoleStatus.addProgressBar(failedReductionIterations/(double)failedIterationLimit);
            if (failedReductionIterations >= failedIterationLimit) {
                std::cout << "\nCouldn't reduce more!\n";
                return 0;
            }
        }
        consoleStatus.display();
    }
}
