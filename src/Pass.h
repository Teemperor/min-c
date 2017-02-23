#ifndef PASS_H
#define PASS_H

#include <string>

typedef int(*plugin_callback)(const char*);

class PassLoadingError {};

class Pass
{

    plugin_callback checkCall;
    plugin_callback transformCall;

public:
    Pass(const std::string& path);

    bool check(const std::string& path);
    bool transform(const std::string& path);

};

#endif // PASS_H
