// Priority queue on top of a binary heap: the value with the highest priority leaves first.
// Highest priority means the largest value, or the smallest value when largestFirst is false.

#ifndef PRIORITYQUEUE_H
#define PRIORITYQUEUE_H

#include "BinaryHeap.h"

class PriorityQueue {
private:
    BinaryHeap heap;

public:
    PriorityQueue(bool largestFirst);

    bool isEmpty() const;
    int size() const;

    bool enqueue(int value);        // false when the heap is full
    bool dequeue(int& value);       // false when empty
    bool peek(int& value) const;

    void clear();
    void display() const;
};

#endif
