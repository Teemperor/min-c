#include "MinCInvocation.h"

#include "Utils.h"

#include <iostream>

MinCInvocation::MinCInvocation(int argc, char **argv) {
  if (argc <= 1) {
    std::cerr << "No args giving. You need to pass the path to the test script."
              << std::endl;
    exit(1);
  }

  testScript = argv[1];

  if (system(nullptr) == 0) {
      std::cerr << "No shell found on system (system(0) == 0)!" << std::endl;
      exit(2);
  }
  if (!Utils::fileExists(testScript)) {
      std::cerr << "Coudln't find test script" << std::endl;
      exit(1);
  }

  for (int argi = 2; argi < argc; ++argi) {
      const std::string arg = argv[argi];
      if (arg.find("--") == 0) {
          std::cerr << "Not supported yet" << std::endl;
          exit(1);
      } else {
      }
    }
}
