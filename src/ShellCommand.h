#ifndef SHELLCOMMAND_H
#define SHELLCOMMAND_H

#include <string>

class ShellCommand {
    std::string output_;
    int exitCode_;
    bool failedToStart_ = false;
public:
    ShellCommand(const std::string& cmd, bool assertSuccess = true);

    struct FailedToRun : public std::exception {
        std::string command;
        virtual const char* what() const noexcept {
            return command.c_str();
        }
    };
    struct CommandFailed  : public std::exception {
        std::string command;
        virtual const char* what() const noexcept {
            return command.c_str();
        }
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
