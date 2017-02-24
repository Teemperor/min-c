#include "Utils.h"

#include <sys/stat.h>
#include <cassert>
#include <fcntl.h>

Utils::Utils()
{

}

bool Utils::copyAttrs(const std::__cxx11::string &pathA, const std::string &pathB) {
    struct stat buf;
    auto origStatResult = stat(pathA.c_str(), &buf);
    assert(origStatResult == 0);

    const struct timespec times[2] {
        buf.st_atim,
        buf.st_mtim
    };

    auto timeResult = utimensat(AT_FDCWD, pathB.c_str(), times, 0);
    assert(timeResult == 0);
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
