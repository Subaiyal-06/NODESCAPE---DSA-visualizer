#include "Trace.h"
#include <iostream>
using namespace std;

namespace Trace {
    bool enabled = false;

    void say(const string& message) {
        if (enabled) {
            cout << "   [step] " << message << endl;
        }
    }
}
