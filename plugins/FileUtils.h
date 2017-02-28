#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <string>
#include <sys/stat.h>
#include <errno.h>
#include <cassert>

long long getFileSize(const std::string &path) {
  struct stat s;
  auto result = stat(path.c_str(), &s);
  if (result == -1 && errno == ENOENT) {
    return -1;
  }
  assert(result == 0);
  return s.st_size;
}


#endif // FILEUTILS_H
