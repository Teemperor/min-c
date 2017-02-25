#ifndef SHELLCOMMAND_H
#define SHELLCOMMAND_H

#include <string>

class ShellCommand {
    std::string output_;
    int exitCode_;
    bool failedToStart_ = false;
public:
    ShellCommand(const std::string& cmd, bool assertSuccess = true);

    struct FailedToRun {
        std::string command;
    };
    struct CommandFailed {
        std::string command;
    };

    const std::string& output() const {
        return output_;
    }

    int exitCode() const {
        return exitCode_;
    }

    bool noRunException() const {
        return failedToStart_ == false;
    }

};


#endif // SHELLCOMMAND_H
