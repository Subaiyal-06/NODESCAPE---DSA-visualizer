#include "CircularQueue.h"
#include "Trace.h"
#include "RecordHelp.h"
#include <iostream>
using namespace std;

namespace {
    const vector<string> CODE_ENQUEUE = {
        "if (count == MAX) return false;     // full",
        "rear = (front + count) % MAX;       // wraps around",
        "arr[rear] = value;  count++;"
    };
    const vector<string> CODE_DEQUEUE = {
        "if (count == 0) return false;       // empty",
        "value = arr[front];",
        "front = (front + 1) % MAX;  count--;"
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

CircularQueue::CircularQueue() {
    front = 0;
    count = 0;
}

bool CircularQueue::isEmpty() const {
    return count == 0;
}

bool CircularQueue::isFull() const {
    return count == MAX;
}

int CircularQueue::size() const {
    return count;
}

bool CircularQueue::enqueue(int value) {
    Recorder::begin("Enqueue (circular queue)", CODE_ENQUEUE);
    if (isFull()) {
        Trace::say("all " + to_string(MAX) + " slots are used, queue is full");
        RecHelp::addSlots(KIND_SLOTS, arr, MAX, RecHelp::usedRange(MAX, front, count), "All " + to_string(MAX) + " slots are used. The queue is full.", 0, markers(front, count, MAX), RecHelp::Hi(front, ROLE_KEPT));
        return false;
    }
    int rear = (front + count) % MAX; // wraps around to 0 after the last slot
    string wrap = (front + count >= MAX) ? " The index wrapped around to the start of the array." : "";
    RecHelp::addSlots(KIND_SLOTS, arr, MAX, RecHelp::usedRange(MAX, front, count), "Next free slot: (front + count) % MAX = " + to_string(rear) + "." + wrap, 1, markers(front, count, MAX), RecHelp::Hi(rear, ROLE_CURRENT));
    arr[rear] = value;
    count++;
    Trace::say("stored " + to_string(value) + " at index " + to_string(rear));
    RecHelp::addSlots(KIND_SLOTS, arr, MAX, RecHelp::usedRange(MAX, front, count), "Store " + to_string(value) + " in slot " + to_string(rear) + ".", 2, markers(front, count, MAX), RecHelp::Hi(rear, ROLE_NEW));
    return true;
}

bool CircularQueue::dequeue(int& value) {
    Recorder::begin("Dequeue (circular queue)", CODE_DEQUEUE);
    if (isEmpty()) {
        Trace::say("queue is empty");
        RecHelp::addSlots(KIND_SLOTS, arr, MAX, RecHelp::usedRange(MAX, front, count), "The queue is empty.", 0, markers(front, count, MAX), RecHelp::Hi());
        return false;
    }
    value = arr[front];
    RecHelp::addSlots(KIND_SLOTS, arr, MAX, RecHelp::usedRange(MAX, front, count), "The value at the front is " + to_string(value) + ". It is returned.", 1, markers(front, count, MAX), RecHelp::Hi(front, ROLE_CURRENT));
    int old = front;
    front = (front + 1) % MAX;
    count--;
    Trace::say("returned " + to_string(value) + ", front moved to " + to_string(front));
    RecHelp::addSlots(KIND_SLOTS, arr, MAX, RecHelp::usedRange(MAX, front, count), "front moves to slot " + to_string(front) + ". Slot " + to_string(old) + " is free and can be reused later.", 2, markers(front, count, MAX), RecHelp::Hi(old, ROLE_REMOVED));
    return true;
}

bool CircularQueue::peekFront(int& value) const {
    if (isEmpty()) {
        return false;
    }
    value = arr[front];
    return true;
}

bool CircularQueue::peekRear(int& value) const {
    if (isEmpty()) {
        return false;
    }
    value = arr[(front + count - 1) % MAX];
    return true;
}

void CircularQueue::clear() {
    front = 0;
    count = 0;
}

void CircularQueue::display() const {
    cout << "Slots: ";
    for (int i = 0; i < MAX; i++) {
        bool used = false;
        for (int k = 0; k < count; k++) {
            if ((front + k) % MAX == i) {
                used = true;
            }
        }
        if (used) cout << "[" << arr[i] << "] ";
        else cout << "[ ] ";
    }
    cout << endl;
    if (isEmpty()) {
        cout << "Queue is empty." << endl;
    }
    else {
        cout << "front index " << front << ", rear index " << (front + count - 1) % MAX << ", size " << count << endl;
    }
}
