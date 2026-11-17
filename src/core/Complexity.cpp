#include "Complexity.h"

const char* complexityName(ComplexityKind kind) {
    switch (kind) {
        case CONSTANT:     return "O(1)";
        case LOGARITHMIC:  return "O(log n)";
        case LINEAR:       return "O(n)";
        case LINEARITHMIC: return "O(n log n)";
        case QUADRATIC:    return "O(n^2)";
        case CUBIC:        return "O(n^3)";
        case EXPONENTIAL:  return "O(2^n)";
        default:           return "?";
    }
}

int ceilLog2(long long n) {
    int k = 0;
    long long value = 1;
    while (value < n) {
        value *= 2;
        k++;
    }
    return k;
}

long long stepsFor(ComplexityKind kind, long long n) {
    if (n < 1) {
        return -1;
    }
    const long long LIMIT = 4000000000000000000LL; // stay inside long long
    switch (kind) {
        case CONSTANT:
            return 1;
        case LOGARITHMIC:
            return ceilLog2(n) + 1;
        case LINEAR:
            return n;
        case LINEARITHMIC:
            if (n > 100000000000LL) return -1;
            return n * (ceilLog2(n) + 1);
        case QUADRATIC:
            if (n > 2000000000LL) return -1;
            return n * n;
        case CUBIC:
            if (n > 1500000LL) return -1;
            return n * n * n;
        case EXPONENTIAL: {
            long long result = 1;
            for (long long i = 0; i < n; i++) {
                if (result > LIMIT / 2) return -1;
                result *= 2;
            }
            return result;
        }
        default:
            return -1;
    }
}
