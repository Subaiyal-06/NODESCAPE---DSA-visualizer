// Tests for DynamicArray and the Time Complexity helpers

#include "TestUtils.h"
#include "../src/core/DynamicArray.h"
#include "../src/core/Complexity.h"

int main() {
    // growing, inserting, removing
    {
        DynamicArray a(4);
        check(a.isEmpty() && a.size() == 0, "new array is empty");
        for (int i = 1; i <= 5; i++) a.append(i * 10);   // 10 20 30 40 50
        check(a.size() == 5 && a.getCapacity() == 8, "capacity doubles when full");
        check(a.insertAt(2, 25), "insertAt middle");      // 10 20 25 30 40 50
        check(a.insertAt(0, 5), "insertAt front");        // 5 10 20 25 30 40 50
        check(a.insertAt(7, 60), "insertAt end");         // ... 50 60
        check(!a.insertAt(-1, 1) && !a.insertAt(9, 1), "insertAt out of range");
        int v;
        check(a.getAt(0, v) && v == 5 && a.getAt(3, v) && v == 25 && a.getAt(7, v) && v == 60, "values after inserts");
        check(a.removeAt(0), "removeAt front");           // 10 20 25 30 40 50 60
        check(a.removeAt(2), "removeAt middle");          // 10 20 30 40 50 60
        check(!a.removeAt(6) && !a.removeAt(-1), "removeAt out of range");
        check(a.size() == 6 && a.getAt(2, v) && v == 30, "values after removes");
        check(!a.getAt(6, v), "getAt out of range");
        a.clear();
        check(a.isEmpty(), "clear");
    }

    // searching
    {
        DynamicArray a;
        for (int i = 1; i <= 10; i++) a.append(i * 5);   // 5 10 ... 50 (sorted)
        check(a.linearSearch(30) == 5, "linear search finds the index");
        check(a.lastComparisons() == 6, "linear search comparisons = index + 1");
        check(a.linearSearch(33) == -1 && a.lastComparisons() == 10, "linear search miss checks every element");
        check(a.binarySearch(35) == 6, "binary search finds the index");
        check(a.binarySearch(5) == 0 && a.binarySearch(50) == 9, "binary search at both ends");
        check(a.binarySearch(33) == -1, "binary search miss");
        check(a.lastComparisons() <= ceilLog2(10) + 1, "binary search needs about log n comparisons");

        DynamicArray b;
        b.append(30); b.append(10); b.append(20);
        check(b.binarySearch(10) == -2, "binary search refuses an unsorted array");
        b.sortAscending();
        check(b.isSorted() && b.binarySearch(10) == 0, "sorted array can be searched");

        DynamicArray empty;
        check(empty.linearSearch(1) == -1 && empty.binarySearch(1) == -1, "search in empty array");
    }

    // complexity helpers
    {
        check(ceilLog2(1) == 0 && ceilLog2(2) == 1 && ceilLog2(5) == 3 && ceilLog2(1024) == 10, "ceilLog2");
        check(stepsFor(CONSTANT, 1000) == 1, "constant steps");
        check(stepsFor(LINEAR, 1000) == 1000, "linear steps");
        check(stepsFor(LOGARITHMIC, 1024) == 11, "logarithmic steps");
        check(stepsFor(LINEARITHMIC, 8) == 8 * 4, "n log n steps");
        check(stepsFor(QUADRATIC, 100) == 10000, "quadratic steps");
        check(stepsFor(CUBIC, 10) == 1000, "cubic steps");
        check(stepsFor(EXPONENTIAL, 10) == 1024, "exponential steps");
        check(stepsFor(EXPONENTIAL, 100) == -1, "exponential overflow is reported");
        check(stepsFor(LINEAR, 0) == -1, "n below 1 is rejected");
    }

    return finishTests();
}
