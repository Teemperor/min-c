
#include "MinCInvocation.h"
#include "Pass.h"
#include "PassManager.h"
#include "DirectoryCopier.h"
#include "PassRunner.h"
#include "ConsoleStatus.h"
#include "ChildProcessDelegate.h"

#include <iostream>
#include <random>
#include <signal.h>

const int failedIterationLimit = 4000000;

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

int main(int argc, char** argv) {
    unsigned jobs = 5;

    PassManager manager;

    std::string binaryDir = argv[0];
    binaryDir = binaryDir.substr(0, binaryDir.find_last_of("/"));
    manager.loadPasses(binaryDir);
    if (manager.passCount() == 0) {
      std::cerr << "Didn't found any passes in " << binaryDir << "! Aborting...";
      return 4;
    }
    std::cout << "[PassManager] Loaded " << manager.passCount() << " passes" << std::endl;


    MinCInvocation invocation(argc, argv);
    invocation.tempDir = "/home/teemperor/min-c/tmpnew/";
    ConsoleStatus consoleStatus(invocation);

    DirectoryCopier copier(".");
    invocation.mainDir = &copier;


    std::vector<ChildProcessDelegate> children;
    std::vector<nlohmann::json> results;
    children.resize(jobs);
    results.resize(jobs);


    unsigned long counter = 0;

    int failedReductionIterations = 0;

    while (true) {
        std::vector<PassRunner> runners;
        for (unsigned long id = 0; id < jobs; id++) {
            Pass& pass = manager.getRandomPass();
            PassRunner runner(id, counter, invocation, pass);
            runners.push_back(runner);

            //std::cout << "[STATUS] Running pass " << pass.name() << " in " << runner.createUniqueDirName() << "\n";
            counter++;
        }

        for (unsigned long id = 0; id < jobs; id++) {
            PassRunner* runner = &runners[id];

            children[id].run([&runner](){
                PassRunner::createRunner(runner);
                nlohmann::json result = runner->toJSON();

                return result;
            });
        }

        bool exit = false;
        while(!exit) {
            exit = true;
            for (auto& worker : children) {
                if (worker.getMessage()) {
                    exit = false;
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        for (unsigned long id = 0; id < jobs; id++) {
            auto& worker = children[id];
            try {
              results[id] = nlohmann::json::parse(worker.message());
              runners[id].updateFromJSON(results[id], manager);
            } catch(const std::invalid_argument&) {
              runners[id].markAsFailed();
            }
        }

        int successes = 0;


        for (PassRunner& runner : runners) {
            if (runner.success()) {
                successes++;
                auto bytesReduced = runner.bytesReduced();
                if (bytesReduced < 0)
                    bytesReduced = 0;
                runner.getPass().adjustWeight(runner.success(), bytesReduced);
            } else {
                runner.getPass().adjustWeight(runner.success(), 0);
            }
        }
        //std::cout << "[STATUS] Had successes: " << successes << "\n";

        const Pass& topPass = manager.getTopPass();
        consoleStatus.appendMessage("Top: " + topPass.name() + " " + std::to_string(topPass.weight()) + " | ");

        consoleStatus.addIteration();

        PassRunner* selectedResult = selectOptimum(runners);
        if (selectedResult) {
            consoleStatus.appendMessage("Reducing... ");
            selectedResult->accept();
            failedReductionIterations = 0;
            if (selectedResult->changedStructure()) {
                invocation.mainDir->recreate();
            }
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
