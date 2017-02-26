#include "ShellCommand.h"
#include <sstream>
#include <fcntl.h>
#include <string.h>
#include <iostream>
#include <thread>

ShellCommand::ShellCommand(const std::string &cmd, bool assertSuccess) {
    FILE *pipe_fp;

    std::string redirectedCommand = cmd + " 2>&1";

    std::stringstream outputStream;

    /* Create one way pipe line with call to popen() */
    if ((pipe_fp = popen(redirectedCommand.c_str(), "r")) == NULL) {
        FailedToRun F;
        F.command = cmd;
        throw F;
    }

    int fd = fileno(pipe_fp);

    int flags;
    flags = fcntl(fd, F_GETFL, 0);
    flags |= O_NONBLOCK;
    fcntl(fd, F_SETFL, flags);

    const size_t bufSize = 80;
    char readbuf[bufSize];

    while (true) {
        auto fgetsResult = fgets(readbuf, bufSize, pipe_fp);
        if (fgetsResult == readbuf) {
            fputs(readbuf, pipe_fp);
            outputStream << readbuf;
        } else {
            if (feof(pipe_fp)) {
                break;
            }
            if (errno == EWOULDBLOCK) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            } else {
                std::cerr << "Failed to read process with " << strerror(errno) << std::endl;
            }
        }
    }


    /* Close the pipe */
    exitCode_ = pclose(pipe_fp);

    failedToStart_ = (exitCode_ == -1);

    if (!failedToStart_) {
        exitCode_ = WEXITSTATUS(exitCode_);
    }

    output_ = outputStream.str();

    if (assertSuccess && exitCode_ != 0) {
        CommandFailed F;
        F.command = cmd;
        throw F;
    }

}
