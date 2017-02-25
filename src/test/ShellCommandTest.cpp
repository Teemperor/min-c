
#include "../ShellCommand.h"

#include <cassert>

int main() {
    ShellCommand cmd("echo foo");
    assert(cmd.output() == "foo\n");

    try {
        ShellCommand cmd2("touch /this/path/should/not/exist/on/your/system/");
        assert(false && "Shouldn't get to this point");
    } catch (const ShellCommand::CommandFailed& f) {
    }

    try {
        ShellCommand cmd2("touch /this/path/should/not/exist/on/your/system/", false);
    } catch (const ShellCommand::CommandFailed& f) {
        assert(false && "Shouldn't get to this point");
    }
}
