#ifndef PASS_H
#define PASS_H

#include <string>

typedef int(*available_callback)();
typedef int(*check_callback)(const char*);
typedef int(*transform_callback)(const char*, unsigned long);

class PassLoadingError {};

class Pass {

    available_callback availableCall;
    check_callback checkCall;
    transform_callback transformCall;
    void *handle;

    std::string name_;

public:
    Pass() {}
    Pass(const std::string& path);

    const std::string& name() const {
        return name_;
    }

    void unload();

    bool check(const std::string& path) const;
    bool transform(const std::string& path, unsigned long random) const;
    bool available() const;

};

#endif // PASS_H
