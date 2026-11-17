// Stack built on a fixed size array

#ifndef ARRAYSTACK_H
#define ARRAYSTACK_H

class ArrayStack {
public:
    static const int MAX = 8;

private:
    int arr[MAX];
    int top; // index of the top element, -1 when empty

public:
    ArrayStack();

    bool isEmpty() const;
    bool isFull() const;
    int size() const;

    bool push(int value);      // false on overflow
    bool pop(int& value);      // false on underflow
    bool peek(int& value) const;

    void clear();
    void display() const;
};

#endif
