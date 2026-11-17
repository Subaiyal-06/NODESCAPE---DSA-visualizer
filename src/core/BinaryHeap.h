// Binary heap stored in an array. For the node at index i: parent = (i - 1) / 2,
// children = 2i + 1 and 2i + 2. A max heap keeps the largest value on top, a min heap the smallest.

#ifndef BINARYHEAP_H
#define BINARYHEAP_H

#include "Sorting.h"

class BinaryHeap {
public:
    static const int MAX = 31;

private:
    int arr[MAX];
    int count;
    bool maxHeap;

    bool comesBefore(int a, int b) const; // true when a must be closer to the top than b
    void siftUp(int index);
    void siftDown(int index);

public:
    BinaryHeap(bool isMaxHeap);

    bool isEmpty() const;
    bool isFull() const;
    int size() const;
    bool isMaxHeap() const;

    bool insert(int value);
    bool extractTop(int& value);       // removes and returns the top value
    bool peek(int& value) const;
    bool buildHeap(const int a[], int n); // bottom-up heapify, false if n > MAX
    bool getAt(int index, int& value) const;
    bool isValidHeap() const;          // used by the tests

    void clear();
    void display() const;              // array form and level by level
};

// Heap sort on a plain array (ascending order), counts comparisons and swaps
void heapSort(int a[], int n, SortStats& stats);

#endif
