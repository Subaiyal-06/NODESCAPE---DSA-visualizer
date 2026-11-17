// Tiny test helpers shared by the test files

#ifndef TESTUTILS_H
#define TESTUTILS_H

#include <iostream>

// the counters live inside functions, so this works with older compilers too
inline int& passedChecks() {
    static int count = 0;
    return count;
}

inline int& failedChecks() {
    static int count = 0;
    return count;
}

inline void check(bool condition, const char* name) {
    if (condition) {
        passedChecks()++;
    } else {
        failedChecks()++;
        std::cout << "FAIL: " << name << std::endl;
    }
}

inline int finishTests() {
    std::cout << passedChecks() << " checks passed, " << failedChecks() << " failed." << std::endl;
    return failedChecks() == 0 ? 0 : 1;
}

#endif
