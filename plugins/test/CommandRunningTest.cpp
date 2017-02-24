#include "../CommandRunning.h"
#include <cassert>

int main() {
    assert(getOutput("echo foo") == "foo\n");
    assert(getOutput("echo -n foo") == "foo");
}
