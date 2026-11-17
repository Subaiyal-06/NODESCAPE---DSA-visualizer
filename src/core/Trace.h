// Step mode: when it is switched on, the core classes explain what they are doing.
// This is the first version of the "step recorder". Later the GUI will replay these steps.

#ifndef TRACE_H
#define TRACE_H

#include <string>

namespace Trace {
    extern bool enabled;                   // set from the main menu
    void say(const std::string& message);  // prints one step when step mode is on
}

#endif
