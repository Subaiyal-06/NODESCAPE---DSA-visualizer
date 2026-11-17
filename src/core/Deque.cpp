#include "Deque.h"
#include "Trace.h"
#include "RecordHelp.h"
#include <iostream>
using namespace std;

namespace {
    const vector<string> CODE_INSERT_FRONT = {
        "if (count == MAX) return false;     // full",
        "front = (front - 1 + MAX) % MAX;    // one step back, wraps around",
        "arr[front] = value;  count++;"
    };
    const vector<string> CODE_INSERT_REAR = {
        "if (count == MAX) return false;     // full",
        "rear = (front + count) % MAX;",
        "arr[rear] = value;  count++;"
    };
    const vector<string> CODE_DELETE_FRONT = {
        "if (count == 0) return false;       // empty",
        "value = arr[front];",
        "front = (front + 1) % MAX;  count--;"
    };
    const vector<string> CODE_DELETE_REAR = {
        "if (count == 0) return false;       // empty",
        "value = arr[(front + count - 1) % MAX];",
        "count--;"
    };

    vector<Marker> markers(int front, int count, int cap) {
        vector<Marker> m;
        if (count > 0) {
            int rear = (front + count - 1) % cap;
            if (rear == front) {
                m.push_back(Marker(front, "front / rear"));
            }
            else {
                m.push_back(Marker(front, "front"));
                m.push_back(Marker(rear, "rear"));
            }
        }
        return m;
    }
}

Deque::Deque() {
    front = 0;
    count = 0;
}

bool Deque::isEmpty() const {
    return count == 0;
}

bool Deque::isFull() const {
    return count == MAX;
}

int Deque::size() const {
    return count;
}

bool Deque::insertFront(int value) {
    Recorder::begin("Insert at front (deque)", CODE_INSERT_FRONT);
    if (isFull()) {
        Trace::say("deque is full");
        RecHelp::addSlots(KIND_SLOTS, arr, MAX, RecHelp::usedRange(MAX, front, count), "The deque is full.", 0, markers(front, count, MAX), RecHelp::Hi(front, ROLE_KEPT));
        return false;
    }
    int oldFront = front;
    front = (front - 1 + MAX) % MAX; // move front one step back, wrapping around
    string wrap = (oldFront == 0 && count > 0) ? " It wrapped around to the end of the array." : "";
    RecHelp::addSlots(KIND_SLOTS, arr, MAX, RecHelp::usedRange(MAX, oldFront, count), "front moves one slot back, to " + to_string(front) + "." + wrap, 1, markers(oldFront, count, MAX), RecHelp::Hi(front, ROLE_CURRENT));
    arr[front] = value;
    count++;
    Trace::say("stored " + to_string(value) + " at the front, index " + to_string(front));
    RecHelp::addSlots(KIND_SLOTS, arr, MAX, RecHelp::usedRange(MAX, front, count), "Store " + to_string(value) + " at the front.", 2, markers(front, count, MAX), RecHelp::Hi(front, ROLE_NEW));
    return true;
}

bool Deque::insertRear(int value) {
    Recorder::begin("Insert at rear (deque)", CODE_INSERT_REAR);
    if (isFull()) {
        Trace::say("deque is full");
        RecHelp::addSlots(KIND_SLOTS, arr, MAX, RecHelp::usedRange(MAX, front, count), "The deque is full.", 0, markers(front, count, MAX), RecHelp::Hi(front, ROLE_KEPT));
        return false;
    }
    int rear = (front + count) % MAX;
    RecHelp::addSlots(KIND_SLOTS, arr, MAX, RecHelp::usedRange(MAX, front, count), "Next free slot after the rear: (front + count) % MAX = " + to_string(rear) + ".", 1, markers(front, count, MAX), RecHelp::Hi(rear, ROLE_CURRENT));
    arr[rear] = value;
    count++;
    Trace::say("stored " + to_string(value) + " at the rear, index " + to_string(rear));
    RecHelp::addSlots(KIND_SLOTS, arr, MAX, RecHelp::usedRange(MAX, front, count), "Store " + to_string(value) + " at the rear.", 2, markers(front, count, MAX), RecHelp::Hi(rear, ROLE_NEW));
    return true;
}

bool Deque::deleteFront(int& value) {
    Recorder::begin("Delete from front (deque)", CODE_DELETE_FRONT);
    if (isEmpty()) {
        Trace::say("deque is empty");
        RecHelp::addSlots(KIND_SLOTS, arr, MAX, RecHelp::usedRange(MAX, front, count), "The deque is empty.", 0, markers(front, count, MAX), RecHelp::Hi());
        return false;
    }
    value = arr[front];
    RecHelp::addSlots(KIND_SLOTS, arr, MAX, RecHelp::usedRange(MAX, front, count), "The front value is " + to_string(value) + ". It is returned.", 1, markers(front, count, MAX), RecHelp::Hi(front, ROLE_CURRENT));
    int old = front;
    front = (front + 1) % MAX;
    count--;
    RecHelp::addSlots(KIND_SLOTS, arr, MAX, RecHelp::usedRange(MAX, front, count), "front moves forward by one. The slot is free again.", 2, markers(front, count, MAX), RecHelp::Hi(old, ROLE_REMOVED));
    return true;
}

bool Deque::deleteRear(int& value) {
    Recorder::begin("Delete from rear (deque)", CODE_DELETE_REAR);
    if (isEmpty()) {
        Trace::say("deque is empty");
        RecHelp::addSlots(KIND_SLOTS, arr, MAX, RecHelp::usedRange(MAX, front, count), "The deque is empty.", 0, markers(front, count, MAX), RecHelp::Hi());
        return false;
    }
    int rear = (front + count - 1) % MAX;
    value = arr[rear];
    RecHelp::addSlots(KIND_SLOTS, arr, MAX, RecHelp::usedRange(MAX, front, count), "The rear value is " + to_string(value) + ". It is returned.", 1, markers(front, count, MAX), RecHelp::Hi(rear, ROLE_CURRENT));
    count--;
    RecHelp::addSlots(KIND_SLOTS, arr, MAX, RecHelp::usedRange(MAX, front, count), "count goes down by one, so the rear moves back. The slot is free again.", 2, markers(front, count, MAX), RecHelp::Hi(rear, ROLE_REMOVED));
    return true;
}

bool Deque::peekFront(int& value) const {
    if (isEmpty()) {
        return false;
    }
    value = arr[front];
    return true;
}

bool Deque::peekRear(int& value) const {
    if (isEmpty()) {
        return false;
    }
    value = arr[(front + count - 1) % MAX];
    return true;
}

void Deque::clear() {
    front = 0;
    count = 0;
}

void Deque::display() const {
    if (isEmpty()) {
        cout << "Deque is empty." << endl;
        return;
    }
    cout << "Front -> ";
    for (int k = 0; k < count; k++) {
        cout << arr[(front + k) % MAX] << " ";
    }
    cout << "<- Rear   (front index " << front << ", size " << count << ")" << endl;
}
