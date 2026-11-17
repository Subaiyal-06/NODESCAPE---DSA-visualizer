// Doubly Linked List - core logic
// Every node knows its previous and its next node. We keep both a head and a tail pointer.

#ifndef DOUBLYLINKEDLIST_H
#define DOUBLYLINKEDLIST_H

class DNode {
public:
    int data;
    DNode* prev; // previous node
    DNode* next; // next node

    DNode(int data) {
        this->data = data;
        this->prev = nullptr;
        this->next = nullptr;
    }
};

class DoublyLinkedList {
private:
    DNode* head;
    DNode* tail;
    int count;

public:
    DoublyLinkedList();
    ~DoublyLinkedList();

    // Copying is switched off, two lists must never share nodes
    DoublyLinkedList(const DoublyLinkedList&) = delete;
    DoublyLinkedList& operator=(const DoublyLinkedList&) = delete;

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

    // Searching: position counted from the head (starting from 1), or -1 if not found
    int search(int key) const;
    // Same search but walking backward from the tail, position counted from the tail
    int searchFromEnd(int key) const;

    void reverse();
    void clear();
    void displayForward() const;
    void displayBackward() const;
};

#endif
