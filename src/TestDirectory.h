#ifndef TESTDIRECTORY_H
#define TESTDIRECTORY_H

#include "MinCInvocation.h"
#include "ShellCommand.h"

class TestDirectory
{
  std::string directory_;
public:
  TestDirectory() {}
  TestDirectory(MinCInvocation& invocation, const std::string &directory);

  void clear();
};

#endif // TESTDIRECTORY_H
