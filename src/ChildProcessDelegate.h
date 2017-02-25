#ifndef CHILDPROCESSDELEGATE_H
#define CHILDPROCESSDELEGATE_H

#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <functional>
#include <vector>
#include <thread>
#include <chrono>
#include <iostream>
#include <fcntl.h>
#include <sstream>
#include <json.hpp>
#include <cassert>

class ChildProcessDelegate {
  static const int WRITE = 1;
  static const int READ = 0;
  std::stringstream readMessage_;
  nlohmann::json json_;
  bool jsonParsed_ = false;
  int pipefd[2];
  bool done = true;
  pid_t cpid = 0;

  void cleanup() {
    waitpid(cpid, NULL, 0);
  }

public:
  void run(std::function<nlohmann::json()> f) {
    assert(done);
    done = false;
    jsonParsed_ = false;
    readMessage_.clear();
    readMessage_.str(std::string());

    pipe(pipefd); // create the pipe
    cpid = fork(); // duplicate the current process
    if (cpid == 0) { // if I am the child then {
        close(pipefd[READ]); // close the read-end of the pipe, I'm not going to use it

        nlohmann::json msg = f();

        std::string output = msg.dump();

        size_t written = 0;

        while (true) {
          auto writeResult = write(pipefd[WRITE], output.c_str() + written, output.size());
          if (write < 0) {
              switch(errno) {
              case EAGAIN:
                  std::this_thread::sleep_for(std::chrono::milliseconds(5));
                  break;
              default:
                  //std::cerr << "FAILED " << strerror(errno) << std::endl;
                  _exit(EXIT_FAILURE);
              }
          } else {
              written += writeResult;
              if (written >= output.size()) {
                  break;
              }
          }
        }

        close(pipefd[WRITE]); // close the write-end of the pipe, thus sending EOF to the reader
        _exit(EXIT_SUCCESS);
    } else {
        close(pipefd[WRITE]); // close the write-end of the pipe, I'm not going to use it
        fcntl(pipefd[READ], F_SETFL, O_NONBLOCK);

        //wait(NULL); // wait for the child process to exit before I do the same
        //exit(EXIT_SUCCESS);
    }
  }


  bool getMessage() {
    if (done)
      return false;
    char buf[100];
    while (true) {
        auto r = read(pipefd[0], &buf, 1);
        if (r == -1 && errno == EAGAIN)
            return true;
        else if (r > 0) {
            buf[r] = 0;
            readMessage_ << buf;
            continue;
        }
        break;
    }
    close(pipefd[0]); // close the read-end of the pipe
    done = true;
    cleanup();
    return false;
  }

  nlohmann::json& getResult() {
    if (!jsonParsed_) {
      json_ = nlohmann::json::parse(message());
    }
    return json_;
  }

  const std::string message() const {
    return readMessage_.str();
  }
};

#endif // CHILDPROCESSDELEGATE_H
