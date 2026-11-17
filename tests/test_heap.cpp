// Tests for the binary heap, heap sort and the priority queue

#include "TestUtils.h"
#include "../src/core/BinaryHeap.h"
#include "../src/core/PriorityQueue.h"
#include <algorithm>
#include <vector>
using namespace std;

static unsigned int seed = 99;
static int nextRandom(int limit) {
    seed = seed * 1103515245u + 12345u;
    return (int)((seed / 65536u) % (unsigned int)limit);
}

int main() {
    // max heap
    {
        BinaryHeap h(true);
        int v;
        check(h.isEmpty() && !h.extractTop(v) && !h.peek(v) && h.isMaxHeap(), "empty max heap");
        int values[] = {5, 3, 17, 10, 84, 19, 6, 22, 9};
        bool valid = true;
        for (int i = 0; i < 9; i++) {
            h.insert(values[i]);
            if (!h.isValidHeap()) valid = false;
        }
        check(valid && h.size() == 9, "heap property after every insert");
        check(h.peek(v) && v == 84, "largest value is on top");
        int expected[] = {84, 22, 19, 17, 10, 9, 6, 5, 3};
        bool order = true;
        for (int i = 0; i < 9; i++) {
            if (!h.extractTop(v) || v != expected[i] || !h.isValidHeap()) order = false;
        }
        check(order && h.isEmpty(), "values leave in descending order");
    }

    // min heap
    {
        BinaryHeap h(false);
        int v;
        int values[] = {40, 10, 30, 20, 50};
        for (int i = 0; i < 5; i++) h.insert(values[i]);
        check(h.peek(v) && v == 10, "smallest value is on top");
        int expected[] = {10, 20, 30, 40, 50};
        bool order = true;
        for (int i = 0; i < 5; i++) {
            if (!h.extractTop(v) || v != expected[i]) order = false;
        }
        check(order, "values leave in ascending order");
    }

    // full heap, build heap
    {
        BinaryHeap h(true);
        bool allIn = true;
        for (int i = 0; i < BinaryHeap::MAX; i++) {
            if (!h.insert(i)) allIn = false;
        }
        check(allIn && h.isFull() && !h.insert(100), "full heap rejects another value");
        h.clear();
        check(h.isEmpty(), "clear");

        int data[] = {4, 10, 3, 5, 1, 8, 7};
        check(h.buildHeap(data, 7) && h.isValidHeap() && h.size() == 7, "buildHeap gives a valid heap");
        int v;
        check(h.peek(v) && v == 10, "buildHeap puts the largest value on top");
        int tooBig[40];
        check(!h.buildHeap(tooBig, 40), "buildHeap rejects too many values");
        check(h.getAt(0, v) && !h.getAt(7, v), "getAt range");
    }

    // heap sort
    {
        bool allSorted = true;
        for (int round = 0; round < 40; round++) {
            int n = nextRandom(50);
            vector<int> input;
            for (int i = 0; i < n; i++) input.push_back(nextRandom(100) - 30);
            vector<int> expected = input;
            sort(expected.begin(), expected.end());
            SortStats stats;
            heapSort(input.data(), n, stats);
            if (input != expected) allSorted = false;
        }
        check(allSorted, "heap sort on 40 random arrays");
        int one[] = {7};
        SortStats stats;
        heapSort(one, 1, stats);
        heapSort(one, 0, stats);
        check(one[0] == 7, "heap sort on tiny arrays");
    }

    // priority queue
    {
        PriorityQueue big(true);
        PriorityQueue small(false);
        int v;
        check(big.isEmpty() && !big.dequeue(v) && !big.peek(v), "empty priority queue");
        int values[] = {30, 10, 50, 20, 40};
        for (int i = 0; i < 5; i++) {
            big.enqueue(values[i]);
            small.enqueue(values[i]);
        }
        check(big.size() == 5 && big.peek(v) && v == 50 && small.peek(v) && v == 10, "highest priority is at the front");
        check(big.dequeue(v) && v == 50 && big.dequeue(v) && v == 40, "largest first");
        check(small.dequeue(v) && v == 10 && small.dequeue(v) && v == 20, "smallest first");
        big.clear();
        check(big.isEmpty(), "clear");
    }

    return finishTests();
}
