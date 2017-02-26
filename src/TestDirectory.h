#ifndef TESTDIRECTORY_H
#define TESTDIRECTORY_H

#include "MinCInvocation.h"
#include "ShellCommand.h"

class TestDirectory
{
  std::string directory;
public:
  TestDirectory() {}
  TestDirectory(MinCInvocation& invocation, std::string directory);

  void clear() {
    ShellCommand("rm -rf '" + directory + "'");
  }
};

#endif // TESTDIRECTORY_H
