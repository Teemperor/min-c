#include "ConsoleStatus.h"

bool ConsoleStatus::checkTimeToPrint() {
    if (invocation_.logMode)
      return true;

    using namespace std::chrono;
    auto millisecondsSinceLastUpdate =
            duration_cast<milliseconds>(steady_clock::now() - lastUpdate);

    if (millisecondsSinceLastUpdate > printInterval) {
        lastUpdate = steady_clock::now();
        return true;
    }
    return false;
}

ConsoleStatus::ConsoleStatus(MinCInvocation &invocation)
  : invocation_(invocation) {
  resetMessage();
}
