// Helpers for the Time Complexity module: how many steps does each growth rate need?

#ifndef COMPLEXITY_H
#define COMPLEXITY_H

enum ComplexityKind {
    CONSTANT,     // O(1)
    LOGARITHMIC,  // O(log n)
    LINEAR,       // O(n)
    LINEARITHMIC, // O(n log n)
    QUADRATIC,    // O(n^2)
    CUBIC,        // O(n^3)
    EXPONENTIAL,  // O(2^n)
    KIND_COUNT
};

const char* complexityName(ComplexityKind kind);
int ceilLog2(long long n);                                   // smallest k with 2^k >= n, ceilLog2(1) = 0
long long stepsFor(ComplexityKind kind, long long n);        // -1 when the number is too big to count

#endif
