// Circular queue: the indexes wrap around, so freed space at the front is reused

#ifndef CIRCULARQUEUE_H
#define CIRCULARQUEUE_H

class CircularQueue {
public:
    static const int MAX = 8;

private:
    int arr[MAX];
    int front; // index of the first item
    int count; // number of items stored

public:
    CircularQueue();

    bool isEmpty() const;
    bool isFull() const;
    int size() const;

    bool enqueue(int value);
    bool dequeue(int& value);
    bool peekFront(int& value) const;
    bool peekRear(int& value) const;

    void clear();
    void display() const;
};

#endif
