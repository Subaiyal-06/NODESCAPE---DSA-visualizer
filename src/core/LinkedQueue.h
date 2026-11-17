// Queue built on a linked list with head (front) and tail (rear) pointers

#ifndef LINKEDQUEUE_H
#define LINKEDQUEUE_H

class QueueNode {
public:
    int data;
    QueueNode* next;

    QueueNode(int data) {
        this->data = data;
        this->next = nullptr;
    }
};

class LinkedQueue {
private:
    QueueNode* head; // front of the queue
    QueueNode* tail; // rear of the queue
    int count;

public:
    LinkedQueue();
    ~LinkedQueue();

    LinkedQueue(const LinkedQueue&) = delete;
    LinkedQueue& operator=(const LinkedQueue&) = delete;

    bool isEmpty() const;
    int size() const;

    void enqueue(int value);
    bool dequeue(int& value);
    bool peekFront(int& value) const;

    void clear();
    void display() const;
};

#endif
