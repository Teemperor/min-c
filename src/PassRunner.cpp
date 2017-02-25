#include "PassRunner.h"

#include "ShellCommand.h"
#include "Utils.h"

#include <algorithm>
#include <stdlib.h>
#include <cassert>
#include <fcntl.h>
#include <unistd.h>


void PassRunner::clearDirectory() {
    ShellCommand("rm -rf '" + directory + "'");
}

long PassRunner::bytesReduced() const {
    assert(success());
    return originalSize - reducedSize;
}

void PassRunner::run() {
    mkdir(directory.c_str(), S_IRWXU);
    invocation_.mainDir->createShallowCopy(directory);

    std::vector<DirectoryCopier::File> targetFiles = invocation_.mainDir->getFileList();

    std::mt19937 randomGenerator(counter_);

    std::shuffle(targetFiles.begin(), targetFiles.end(), randomGenerator);

    std::string targetFilePath;
    bool transformed = false;
    for (const auto& targetFile : targetFiles) {
        targetFilePath = directory + "/" + targetFile.filePath;
        if (pass_.check(targetFilePath)) {
            invocation_.mainDir->createDeepCopy(directory, targetFile);
            originalSize = Utils::getFileSize(targetFilePath);

            if (pass_.transform(targetFilePath, randomGenerator())) {
                modifiedFile = targetFile.filePath;
                reducedSize = Utils::getFileSize(targetFilePath);
                transformed = true;
                break;
            }
        }
    }
    if (transformed) {
        if (Utils::fileExists(targetFilePath)) {
            Utils::copyAttrs(modifiedFile, targetFilePath);
        }
        runTestCommand();
    } else {
        success_ = false;
    }

}

void PassRunner::accept() {
    assert(success());

    struct stat buf;
    auto origStatResult = stat(modifiedFile.c_str(), &buf);
    assert(origStatResult == 0);

    const DirectoryCopier::File& file = invocation_.mainDir->getFileByFilepath(modifiedFile);
    auto unlinkResult = unlink(modifiedFile.c_str());

    assert(unlinkResult == 0);

    std::string runnerFilePath = directory + "/" + modifiedFile;
    struct stat unusedBuf;
    auto statResult = stat(runnerFilePath.c_str(), &unusedBuf);
    if (statResult == -1) {
        changedStructure_ = true;
        return;
    }

    auto linkResult = link(runnerFilePath.c_str(), file.filePath.c_str());

    assert(linkResult == 0);

    const struct timespec times[2] {
        buf.st_atim,
        buf.st_mtim
    };

    utimensat(AT_FDCWD, modifiedFile.c_str(), times, 0);

    if (invocation_.reverifyRuns) {

        ShellCommand("bash '" +invocation_.testScript + "' .");

        /*auto exitCode = system(command.c_str());
        if (exitCode == -1) {
            std::cerr << "Command " << command << " failed to execute" << std::endl;
        }
        exitCode = WEXITSTATUS(exitCode);
        if (exitCode) {
            std::cerr << "\n\nFAILED " << this->pass_.name() << " " << createUniqueDirName() << " " << exitCode << std::endl;
            system(("cd '" + directory + "' && cat err").c_str());
        }
        assert(exitCode == 0); */
    }
}

#include <regex>
#include <mutex>
std::mutex g_pages_mutex;

void PassRunner::runTestCommand() {
    std::lock_guard<std::mutex> guard(g_pages_mutex);

    ShellCommand cmd("cd '" + directory + "' && bash '" +
                 invocation_.testScript + "' '" + directory + "'", false);
    success_ = (cmd.exitCode() == 0 && cmd.noRunException());
    //std::cerr << cmd.output() << std::endl;

    std::regex color_regex("INSIDEN: (\\S+)");
    std::regex color_regex1("(PPID: \\S+)");
    std::regex color_regex2("(PID: \\S+)");
    std::regex color_regex3("(TPID: \\S+)");

    std::smatch color_match;
    if(std::regex_search(cmd.output(), color_match, color_regex)) {
        std::string noSlashDir = directory;
        noSlashDir.erase(std::remove(noSlashDir.begin(), noSlashDir.end(), '/'), noSlashDir.end());

        std::string noSlashDir2 = color_match[1];
        noSlashDir2.erase(std::remove(noSlashDir2.begin(), noSlashDir2.end(), '/'), noSlashDir2.end());
        std::cout << '\n' << directory << "==" << color_match[1] << '\n';
        assert(noSlashDir== noSlashDir2);
    } else {
        //assert(false);
    }
    /*if(std::regex_search(cmd.output(), color_match, color_regex2))
        std::cout << id_ << color_match[1] << ' ';
    if(std::regex_search(cmd.output(), color_match, color_regex1))
        std::cout << id_ << color_match[1] << ' ';
    if(std::regex_search(cmd.output(), color_match, color_regex3))
        std::cout << id_ << color_match[1] << '\n'; */

    /*
    const std::string command = ; ;// " 1>/dev/null 2>/dev/null";
    std::cerr << "RUNNING: " << std::endl << command << std::endl;
    auto exitCode = system(command.c_str());
    if (exitCode == -1) {
        std::cerr << "Command " << command << " failed to execute" << std::endl;
    }
    exitCode = WEXITSTATUS(exitCode);
    success_ = (exitCode == 0);

    if (invocation_.reverifyRuns) {
        std::cout << std::endl << createUniqueDirName() << " EXIT: " << exitCode << std::endl;
    } */
}
