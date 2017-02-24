#include "Utils.h"

#include <sys/stat.h>
#include <cassert>

Utils::Utils()
{

}

bool Utils::fileExists(const std::__cxx11::string &path) {
  struct stat s;
  auto result = stat(path.c_str(), &s);
  if (result == -1) {
    return false;
  }
  return true;
}

long long Utils::getFileSize(const std::string &path) {
  struct stat s;
  auto result = stat(path.c_str(), &s);
  if (result == -1 && errno == ENOENT) {
    return -1;
  }
  assert(result == 0);
  return s.st_size;
}
