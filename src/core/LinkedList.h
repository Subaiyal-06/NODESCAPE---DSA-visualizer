// Singly Linked List - core logic
// This file only holds the data structure itself. Menus and input live in src/ui.

#ifndef LINKEDLIST_H
#define LINKEDLIST_H

class Node {
public:
    int data;   // value stored in this node
    Node* next; // pointer to the next node

    Node(int data) {
        this->data = data;
        this->next = nullptr;
    }
};

class LinkedList {
private:
    Node* head; // first node of the list
    int count;  // number of nodes, so size() does not need a traversal

    // the plain linking steps, shared by several public operations (they record no frames)
    void pushFront(int data);
    bool popFront();

public:
    LinkedList();
    ~LinkedList();

    // Copying a list would make two lists share the same nodes, so it is switched off
    LinkedList(const LinkedList&) = delete;
    LinkedList& operator=(const LinkedList&) = delete;

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

    // Searching: position of the first match (starting from 1), or -1 if not found
    int search(int key) const;

    // Other operations
    void reverse();
    void sortAscending();
    void clear();
    void display() const;
};

#endif
