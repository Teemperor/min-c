#ifndef UTILS_H
#define UTILS_H

#include <string>

class Utils
{
public:
  Utils();

  static bool fileExists(const std::string& path);

  static long long getFileSize(const std::string& path);
};

#endif // UTILS_H
