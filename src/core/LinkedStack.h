// Stack built on a linked list. The top of the stack is the head of the list.

#ifndef LINKEDSTACK_H
#define LINKEDSTACK_H

class StackNode {
public:
    int data;
    StackNode* next;

    StackNode(int data) {
        this->data = data;
        this->next = nullptr;
    }
};

class LinkedStack {
private:
    StackNode* top;
    int count;

public:
    LinkedStack();
    ~LinkedStack();

    LinkedStack(const LinkedStack&) = delete;
    LinkedStack& operator=(const LinkedStack&) = delete;

    bool isEmpty() const;
    int size() const;

    void push(int value);
    bool pop(int& value);       // false on underflow
    bool peek(int& value) const;

    void clear();
    void display() const;
};

#endif
