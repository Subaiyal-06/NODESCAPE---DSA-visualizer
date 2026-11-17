// Tests for all sorting algorithms. Every algorithm is checked against std::sort.

#include "TestUtils.h"
#include "../src/core/Sorting.h"
#include <algorithm>
#include <vector>
#include <string>
using namespace std;

typedef void (*SortFunction)(int[], int, SortStats&);

// small deterministic random numbers, so the tests give the same result every time
static unsigned int seed = 12345;
static int nextRandom(int limit) {
    seed = seed * 1103515245u + 12345u;
    return (int)((seed / 65536u) % (unsigned int)limit);
}

static bool sortsCorrectly(SortFunction sorter, const vector<int>& input) {
    vector<int> expected = input;
    sort(expected.begin(), expected.end());
    vector<int> actual = input;
    SortStats stats;
    sorter(actual.data(), (int)actual.size(), stats);
    return actual == expected;
}

int main() {
    const char* names[] = {"bubble", "selection", "insertion", "merge", "quick"};
    SortFunction sorters[] = {bubbleSort, selectionSort, insertionSort, mergeSort, quickSort};

    for (int s = 0; s < 5; s++) {
        string name = names[s];
        check(sortsCorrectly(sorters[s], {}), (name + ": empty array").c_str());
        check(sortsCorrectly(sorters[s], {7}), (name + ": one element").c_str());
        check(sortsCorrectly(sorters[s], {2, 1}), (name + ": two elements").c_str());
        check(sortsCorrectly(sorters[s], {1, 2, 3, 4, 5, 6}), (name + ": already sorted").c_str());
        check(sortsCorrectly(sorters[s], {9, 8, 7, 6, 5, 4, 3}), (name + ": reversed").c_str());
        check(sortsCorrectly(sorters[s], {4, 4, 4, 4}), (name + ": all equal").c_str());
        check(sortsCorrectly(sorters[s], {5, 1, 5, 2, 1, 2, 5}), (name + ": duplicates").c_str());
        check(sortsCorrectly(sorters[s], {-3, 5, -10, 0, 8}), (name + ": negative numbers").c_str());

        bool allRandomOk = true;
        for (int round = 0; round < 40; round++) {
            int n = nextRandom(60);
            vector<int> input;
            for (int i = 0; i < n; i++) input.push_back(nextRandom(100) - 20);
            if (!sortsCorrectly(sorters[s], input)) allRandomOk = false;
        }
        check(allRandomOk, (name + ": 40 random arrays").c_str());
    }

    // the counters
    {
        int sorted[] = {1, 2, 3, 4, 5};
        SortStats st;
        bubbleSort(sorted, 5, st);
        check(st.comparisons == 4 && st.swaps == 0, "bubble sort stops after one pass on sorted data");

        int reversed[] = {5, 4, 3, 2, 1};
        SortStats st2;
        bubbleSort(reversed, 5, st2);
        check(st2.swaps == 10, "bubble sort swaps on reversed data (n(n-1)/2)");

        int data[] = {3, 1, 4, 1, 5, 9, 2, 6};
        SortStats st3;
        selectionSort(data, 8, st3);
        check(st3.comparisons == 28, "selection sort always makes n(n-1)/2 comparisons");

        int sorted2[] = {1, 2, 3, 4, 5};
        SortStats st4;
        insertionSort(sorted2, 5, st4);
        check(st4.comparisons == 4 && st4.swaps == 0, "insertion sort on sorted data is linear");

        int unsorted[] = {3, 1, 2};
        check(isSortedAscending(sorted2, 5), "isSortedAscending on sorted data");
        check(!isSortedAscending(unsorted, 3), "isSortedAscending on unsorted data");
    }

    return finishTests();
}
