#include "PriorityQueue.h"

PriorityQueue::PriorityQueue(bool largestFirst) : heap(largestFirst) {
}

bool PriorityQueue::isEmpty() const {
    return heap.isEmpty();
}

int PriorityQueue::size() const {
    return heap.size();
}

bool PriorityQueue::enqueue(int value) {
    return heap.insert(value);
}

bool PriorityQueue::dequeue(int& value) {
    return heap.extractTop(value);
}

bool PriorityQueue::peek(int& value) const {
    return heap.peek(value);
}

void PriorityQueue::clear() {
    heap.clear();
}

void PriorityQueue::display() const {
    heap.display();
}
