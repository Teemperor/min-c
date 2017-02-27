#include "PassRunner.h"

#include "ShellCommand.h"
#include "Utils.h"
#include "PassManager.h"

#include <algorithm>
#include <stdlib.h>
#include <cassert>
#include <fcntl.h>
#include <unistd.h>


void PassRunner::clearDirectory() {
    testDirectory_.clear();
}

long PassRunner::bytesReduced() const {
    return originalSize - reducedSize;
}

void PassRunner::run() {
    std::vector<DirectoryCopier::File> targetFiles = invocation_.mainDir->getFileList();

    std::mt19937 randomGenerator(counter_);

    std::shuffle(targetFiles.begin(), targetFiles.end(), randomGenerator);

    std::string targetFilePath;
    bool transformed = false;
    for (const auto& targetFile : targetFiles) {
        targetFilePath = directory + "/" + targetFile.filePath;
        if (pass_->check(targetFilePath)) {
            invocation_.mainDir->createDeepCopy(directory, targetFile);
            originalSize = Utils::getFileSize(targetFilePath);

            if (pass_->transform(targetFilePath, randomGenerator())) {
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

void PassRunner::runTestCommand() {

    ShellCommand cmd("cd '" + directory + "' && bash '" +
                 invocation_.testScript + "' '" + directory + "'", false);
    success_ = (cmd.exitCode() == 0 && cmd.noRunException());

    std::regex color_regex("INSIDE: (\\S+)");

    std::smatch color_match;
    if(std::regex_search(cmd.output(), color_match, color_regex)) {
        std::string noSlashDir = directory;
        noSlashDir.erase(std::remove(noSlashDir.begin(), noSlashDir.end(), '/'), noSlashDir.end());

        std::string noSlashDir2 = color_match[1];
        noSlashDir2.erase(std::remove(noSlashDir2.begin(), noSlashDir2.end(), '/'), noSlashDir2.end());
        //std::cerr << '\n' << directory << "==" << color_match[1] << '\n';
        assert(noSlashDir== noSlashDir2);
    }
}

PassRunner::PassRunner(unsigned long id, unsigned long counter, MinCInvocation &invocation, Pass &pass)
  : id_(id), counter_(counter), invocation_(invocation), pass_(&pass) {
  directory = invocation.tempDir + "/" + createUniqueDirName() + "/";
  testDirectory_ = TestDirectory(invocation_, invocation_.tempDir + "/" + createUniqueDirName());
}

nlohmann::json PassRunner::toJSON() {
  nlohmann::json result;
  result["success"] = success_;
  result["file"] = modifiedFile;
  result["directory"] = directory;
  result["pass"] = pass_->name();
  result["changedStructure"] = changedStructure_;
  result["originalSize"] = originalSize;
  result["reducedSize"] = reducedSize;
  return result;
}

void PassRunner::updateFromJSON(nlohmann::json &json, PassManager &manager) {
  success_ = json["success"];
  modifiedFile = json["file"];
  directory = json["directory"];
  pass_ = &manager.getPass(json["pass"]);
  changedStructure_ = json["changedStructure"];
  originalSize = json["originalSize"];
  reducedSize = json["reducedSize"];
}
