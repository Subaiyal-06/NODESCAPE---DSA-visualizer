// Circular Linked List - core logic
// The last node points back to the first node. We keep only a tail pointer:
// tail is the last node and tail->next is the first node.

#ifndef CIRCULARLINKEDLIST_H
#define CIRCULARLINKEDLIST_H

class CNode {
public:
    int data;
    CNode* next;

    CNode(int data) {
        this->data = data;
        this->next = nullptr;
    }
};

class CircularLinkedList {
private:
    CNode* tail; // last node, tail->next is the first node
    int count;

public:
    CircularLinkedList();
    ~CircularLinkedList();

    CircularLinkedList(const CircularLinkedList&) = delete;
    CircularLinkedList& operator=(const CircularLinkedList&) = delete;

    bool isEmpty() const;
    int size() const;

    // Insertion (positions start from 1)
    void insertAtBeginning(int data);
    void insertAtEnd(int data);
    bool insertAtPosition(int position, int data);

    // Deletion (each one returns false when nothing could be deleted)
    bool deleteFirst();
    bool deleteLast();
    bool deleteByValue(int key);
    bool deleteAtPosition(int position);

    // Position of the first match (starting from 1), or -1 if not found
    int search(int key) const;

    // Used by the tests to prove that the last node really links back to the first
    bool getFirst(int& value) const;
    bool getLast(int& value) const;
    bool getNextOfLast(int& value) const;

    void clear();
    void display() const;
};

#endif
