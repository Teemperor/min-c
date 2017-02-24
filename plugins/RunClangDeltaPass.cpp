
#include "GenericFilter.h"

#include "CommandRunning.h"

#include <string>
#include <fstream>
#include <streambuf>
#include <iostream>
#include <regex>
#include <random>

#define CLANG_DELTA "/usr/lib/creduce/clang_delta"

extern "C" {
    int available() {
        if (system(CLANG_DELTA " --help 1>/dev/null") == 0) {
            return 1; // works
        } else {
            return 0; // doesn't works
        }
    }

    int check(const char* path) {
        return commonFilter(path);
    }

    int transform(const char* path, unsigned long random) {
      std::vector<std::string> transformations;

      {
        std::istringstream iss(getOutput(CLANG_DELTA " --transformations"));
        std::string s;
        while (std::getline(iss, s, '\n')) {
            transformations.push_back(s);
        }
      }

      std::mt19937 r(random);

      std::shuffle(transformations.begin(), transformations.end(), r);

      std::regex rgx("[^\\d]*(\\d+)[^\\d]*");

      unsigned availableCounters = 0;

      for (std::string transformation : transformations) {
          std::istringstream iss(getOutput(CLANG_DELTA " --query-instances=" +
                                            transformation + " '"
                                            + std::string(path) + "'"));
          std::string s = iss.str();

          std::smatch match;

          if (std::regex_search(s, match, rgx)) {
              std::string availableCountersStr = match[1];
              availableCounters = std::atoi(availableCountersStr.c_str());
          } else {
              // Unsupported file type probably
              return 0;
          }

          if (availableCounters == 0)
            continue;

          unsigned counter = random % availableCounters;

          const std::string transformCommand = CLANG_DELTA " --transformation=" +
              transformation + " --counter=" + std::to_string(counter) + " '" +
              std::string(path) + "' >'" + std::string(path) + ".red'";
          if (system(transformCommand.c_str()))
              return 0;
          const std::string mvCommand = "mv '" + std::string(path) + ".red' '" + std::string(path) + "'";
          if (system(mvCommand.c_str()))
              return 0;

          return 1;
      }

      return 0;
    }
}
