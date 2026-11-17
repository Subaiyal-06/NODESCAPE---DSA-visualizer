#include "LinkedQueue.h"
#include "Trace.h"
#include "RecordHelp.h"
#include <iostream>
using namespace std;

namespace {
    const vector<string> CODE_ENQUEUE = {
        "Node* n = new Node(value);",
        "if (tail == NULL) head = n;",
        "else tail->next = n;",
        "tail = n;"
    };
    const vector<string> CODE_DEQUEUE = {
        "if (head == NULL) return false;     // empty",
        "Node* t = head; value = t->data;",
        "head = head->next;",
        "if (head == NULL) tail = NULL;",
        "delete t;"
    };

    vector<int> valuesOf(QueueNode* head) {
        vector<int> v;
        for (QueueNode* t = head; t != nullptr; t = t->next) v.push_back(t->data);
        return v;
    }

    vector<Marker> markers(size_t n) {
        vector<Marker> m;
        if (n == 1) {
            m.push_back(Marker(0, "front / rear"));
        }
        else if (n > 1) {
            m.push_back(Marker(0, "front"));
            m.push_back(Marker((int)n - 1, "rear"));
        }
        return m;
    }
}

LinkedQueue::LinkedQueue() {
    head = nullptr;
    tail = nullptr;
    count = 0;
}

LinkedQueue::~LinkedQueue() {
    clear();
}

bool LinkedQueue::isEmpty() const {
    return head == nullptr;
}

int LinkedQueue::size() const {
    return count;
}

void LinkedQueue::enqueue(int value) {
    Recorder::begin("Enqueue (linked queue)", CODE_ENQUEUE);
    if (Recorder::enabled()) {
        vector<int> before = valuesOf(head);
        RecHelp::addValues(KIND_LIST, before, "Create a new node holding " + to_string(value) + ".", 0, markers(before.size()), RecHelp::Hi(), true, value);
    }
    QueueNode* newNode = new QueueNode(value);
    if (tail == nullptr) {
        head = newNode;
        tail = newNode;
    }
    else {
        tail->next = newNode;
        tail = newNode;
    }
    count++;
    Trace::say("added " + to_string(value) + " at the rear");
    if (Recorder::enabled()) {
        vector<int> after = valuesOf(head);
        RecHelp::addValues(KIND_LIST, after, "The old rear links to the new node and rear moves to it. No walking: O(1).", 3, markers(after.size()), RecHelp::Hi((int)after.size() - 1, ROLE_NEW));
    }
}

bool LinkedQueue::dequeue(int& value) {
    Recorder::begin("Dequeue (linked queue)", CODE_DEQUEUE);
    if (head == nullptr) {
        Trace::say("head is NULL, queue is empty");
        RecHelp::addValues(KIND_LIST, vector<int>(), "head is NULL: the queue is empty.", 0, vector<Marker>(), RecHelp::Hi());
        return false;
    }
    vector<int> before = valuesOf(head);
    RecHelp::addValues(KIND_LIST, before, "Remember the front node " + to_string(head->data) + ".", 1, markers(before.size()), RecHelp::Hi(0, ROLE_CURRENT));
    QueueNode* temp = head;
    value = temp->data;
    head = head->next;
    if (head == nullptr) {
        tail = nullptr; // the queue became empty
    }
    RecHelp::addValues(KIND_LIST, before, "front moves to the next node. The old front is about to be freed.", 2, markers(before.size()), RecHelp::Hi(0, ROLE_REMOVED));
    delete temp;
    count--;
    Trace::say("removed " + to_string(value) + " from the front");
    vector<int> after = valuesOf(head);
    RecHelp::addValues(KIND_LIST, after, "Node deleted. " + to_string(value) + " is returned.", 4, markers(after.size()), RecHelp::Hi());
    return true;
}

bool LinkedQueue::peekFront(int& value) const {
    if (head == nullptr) {
        return false;
    }
    value = head->data;
    return true;
}

void LinkedQueue::clear() {
    while (head != nullptr) { // give every node back without recording anything
        QueueNode* temp = head;
        head = head->next;
        delete temp;
    }
    tail = nullptr;
    count = 0;
}

void LinkedQueue::display() const {
    if (head == nullptr) {
        cout << "Queue is empty." << endl;
        return;
    }
    cout << "Front -> ";
    QueueNode* temp = head;
    while (temp != nullptr) {
        cout << temp->data << " -> ";
        temp = temp->next;
    }
    cout << "NULL   (rear is " << tail->data << ")" << endl;
}
