#include "TestDirectory.h"

#include <iostream>
#include <cassert>

TestDirectory::TestDirectory(MinCInvocation &invocation, const std::string& directory)
  : directory_(directory) {
  assert(!directory_.empty());
  mkdir(directory.c_str(), S_IRWXU);
  invocation.mainDir->createShallowCopy(directory);
}

void TestDirectory::clear() {
  assert(!directory_.empty());
  ShellCommand("rm -rf '" + directory_ + "'");
}
