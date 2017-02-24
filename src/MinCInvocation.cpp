#include "MinCInvocation.h"

#include "Utils.h"

#include <iostream>
#include <unistd.h>
#include <limits.h>
#include <cassert>

MinCInvocation::MinCInvocation(int argc, char **argv) {
  if (argc <= 1) {
    std::cerr << "No args giving. You need to pass the path to the test script."
              << std::endl;
    exit(1);
  }

  testScript = argv[1];



  if (searchtmpfs) {
      if (Utils::fileExists("/dev/shm/")) {
          std::cout << "[Info] Using tmpfs in /dev/shm/" << std::endl;
          tmpfs = "/dev/shm/";
          tempDir = tmpfs + "minc/";
          std::string newSrcPath = tempDir + "src";
          mkdir(tempDir.c_str(), S_IRWXU);
          system(("cp -r . '" + newSrcPath + "'").c_str());
          chdir(newSrcPath.c_str());
          char buf[2048];
          auto cwdresult = getcwd(buf, 2048);
          assert(cwdresult);
          origSrcDir = buf;
      }
  }

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

MinCInvocation::~MinCInvocation() {
    if (!origSrcDir.empty()) {
        std::cerr << std::endl << std::endl << "NOOO" << std::endl;
        //system(("rm -rf . '" + origSrcDir + "'").c_str());
        //system(("cp -r . '" + origSrcDir + "'").c_str());
    }
}
