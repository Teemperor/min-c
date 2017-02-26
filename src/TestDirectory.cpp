#include "TestDirectory.h"

TestDirectory::TestDirectory(MinCInvocation &invocation, std::string directory)
  : directory(directory) {
  mkdir(directory.c_str(), S_IRWXU);
  invocation.mainDir->createShallowCopy(directory);
}
