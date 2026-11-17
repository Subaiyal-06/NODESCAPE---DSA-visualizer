// Double ended queue: items can be added and removed at both ends. Circular array inside.

#ifndef DEQUE_H
#define DEQUE_H

class Deque {
public:
    static const int MAX = 8;

private:
    int arr[MAX];
    int front;
    int count;

public:
    Deque();

    bool isEmpty() const;
    bool isFull() const;
    int size() const;

    bool insertFront(int value);
    bool insertRear(int value);
    bool deleteFront(int& value);
    bool deleteRear(int& value);
    bool peekFront(int& value) const;
    bool peekRear(int& value) const;

    void clear();
    void display() const;
};

#endif
