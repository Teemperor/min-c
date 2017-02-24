
#include <cassert>
#include "../GenericFilter.h"

int main() {
    assert(!commonFilter("/test/module.modulemap"));
    assert(!commonFilter("clang-5.0"));
    assert(!commonFilter("clang-3.9"));
    assert(!commonFilter("/chroot.so"));
    assert(!commonFilter("/test.sh"));
    assert(!commonFilter("/run.sh"));
    assert(commonFilter("/bin/clang-101"));
    assert(commonFilter("test.h"));
    assert(commonFilter("test.cpp"));
    assert(commonFilter("test.cxx"));
    assert(commonFilter("/bin/clang-3.9/../iostream"));
    assert(commonFilter("include/iostream"));
}
