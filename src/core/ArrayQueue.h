// Simple (linear) queue on an array. Once rear reaches the end no new item fits,
// even if items were removed from the front. The circular queue fixes this.

#ifndef ARRAYQUEUE_H
#define ARRAYQUEUE_H

class ArrayQueue {
public:
    static const int MAX = 8;

private:
    int arr[MAX];
    int front; // index of the first item
    int rear;  // index of the last item, front > rear means empty

public:
    ArrayQueue();

    bool isEmpty() const;
    bool isFull() const;
    int size() const;

    bool enqueue(int value);      // false when rear is at the end of the array
    bool dequeue(int& value);     // false when empty
    bool peekFront(int& value) const;

    void clear();
    void display() const;
};

#endif
