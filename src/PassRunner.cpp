#include "PassRunner.h"

#include <algorithm>
#include <stdlib.h>
#include <cassert>
#include <unistd.h>

void PassRunner::clearDirectory() {
    const std::string deleteCommand = "rm -rf '" + directory + "'";
    system(deleteCommand.c_str());
}

long PassRunner::bytesReduced() const {
    assert(success());
    return originalSize - reducedSize;
}

static long long getFileSize(const std::string& path) {
    struct stat s;
    auto result = stat(path.c_str(), &s);
    if (result == -1 && errno == ENOENT) {
        return -20;
    }
    assert(result == 0);
    return s.st_size;
}

void PassRunner::run() {
    mkdir(directory.c_str(), S_IRWXU);
    invocation_.mainDir->createShallowCopy(directory);
    std::vector<DirectoryCopier::File> targetFiles = invocation_.mainDir->getFileList();

    std::mt19937 randomGenerator(counter_);

    std::shuffle(targetFiles.begin(), targetFiles.end(), randomGenerator);

    bool transformed = false;
    for (const auto& targetFile : targetFiles) {
        std::string targetFilePath = directory + "/" + targetFile.filePath;
        if (pass_.check(targetFilePath)) {
            invocation_.mainDir->createDeepCopy(directory, targetFile);
            originalSize = getFileSize(targetFilePath);
            if (pass_.transform(targetFilePath, randomGenerator())) {
                modifiedFile = targetFile.filePath;
                reducedSize = getFileSize(targetFilePath);
                transformed = true;
                break;
            }
        }
    }
    if (transformed) {
        runTestCommand();
    } else {
        success_ = false;
    }

}

void PassRunner::accept() {
    assert(success());

    const DirectoryCopier::File& file = invocation_.mainDir->getFileByFilepath(modifiedFile);
    unlink(modifiedFile.c_str());

    std::string runnerFilePath = directory + "/" + modifiedFile;
    struct stat buf;
    auto statResult = stat(runnerFilePath.c_str(), &buf);
    if (statResult == -1) {
        changedStructure_ = true;
        for (int i = 0; i < 1000; i++)
            std::cout << "CHANGED STRUCTURE" << std::endl;
        return;
    }

    link(runnerFilePath.c_str(), file.filePath.c_str());

}

void PassRunner::runTestCommand() {
    const std::string command = "cd '" + directory + "' && bash '" + invocation_.testScript + "'";
    auto exitCode = system(command.c_str());
    success_ = (exitCode == 0);
}
