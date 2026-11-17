#include "ArrayQueue.h"
#include "Trace.h"
#include "RecordHelp.h"
#include <iostream>
using namespace std;

namespace {
    const vector<string> CODE_ENQUEUE = {
        "if (rear == MAX - 1) return false;  // full, even if there is space in front",
        "rear++;",
        "arr[rear] = value;"
    };
    const vector<string> CODE_DEQUEUE = {
        "if (front > rear) return false;     // empty",
        "value = arr[front];",
        "front++;                            // the freed slot is never used again"
    };

    vector<char> usedRange(int front, int rear, int cap) {
        vector<char> used(cap, 0);
        for (int i = front; i <= rear && i < cap; i++) if (i >= 0) used[i] = 1;
        return used;
    }

    vector<Marker> markers(int front, int rear) {
        vector<Marker> m;
        if (front <= rear) {
            m.push_back(Marker(front, "front"));
            m.push_back(Marker(rear, "rear"));
        }
        return m;
    }
}

ArrayQueue::ArrayQueue() {
    front = 0;
    rear = -1;
}

bool ArrayQueue::isEmpty() const {
    return front > rear;
}

bool ArrayQueue::isFull() const {
    return rear == MAX - 1;
}

int ArrayQueue::size() const {
    return rear - front + 1;
}

bool ArrayQueue::enqueue(int value) {
    Recorder::begin("Enqueue (simple queue)", CODE_ENQUEUE);
    if (isFull()) {
        Trace::say("rear = MAX - 1, queue is full");
        string why = front > 0 ? "rear is at the last slot, so the queue is full even though " + to_string(front) + " slot(s) in front are free. That is the weakness of a simple array queue." : "The queue is full (rear = MAX - 1).";
        RecHelp::addSlots(KIND_SLOTS, arr, MAX, usedRange(front, rear, MAX), why, 0, markers(front, rear), RecHelp::Hi(rear, ROLE_KEPT));
        return false;
    }
    rear++;
    RecHelp::addSlots(KIND_SLOTS, arr, MAX, usedRange(front, rear - 1, MAX), "rear moves one slot to the right, to index " + to_string(rear) + ".", 1, markers(front, rear), RecHelp::Hi(rear, ROLE_CURRENT));
    arr[rear] = value;
    Trace::say("rear moved to " + to_string(rear) + ", stored " + to_string(value));
    RecHelp::addSlots(KIND_SLOTS, arr, MAX, usedRange(front, rear, MAX), "Store " + to_string(value) + " in arr[" + to_string(rear) + "].", 2, markers(front, rear), RecHelp::Hi(rear, ROLE_NEW));
    return true;
}

bool ArrayQueue::dequeue(int& value) {
    Recorder::begin("Dequeue (simple queue)", CODE_DEQUEUE);
    if (isEmpty()) {
        Trace::say("front passed rear, queue is empty");
        RecHelp::addSlots(KIND_SLOTS, arr, MAX, usedRange(front, rear, MAX), "The queue is empty (front is past rear).", 0, markers(front, rear), RecHelp::Hi());
        return false;
    }
    value = arr[front];
    RecHelp::addSlots(KIND_SLOTS, arr, MAX, usedRange(front, rear, MAX), "The value at the front is " + to_string(value) + ". It is returned.", 1, markers(front, rear), RecHelp::Hi(front, ROLE_CURRENT));
    int old = front;
    front++;
    Trace::say("returned " + to_string(value) + ", front moved to " + to_string(front));
    RecHelp::addSlots(KIND_SLOTS, arr, MAX, usedRange(front, rear, MAX), "front moves one slot to the right. Slot " + to_string(old) + " is now wasted: this queue never reuses it.", 2, markers(front, rear), RecHelp::Hi(old, ROLE_REMOVED));
    if (front > rear) {
        front = 0; // the queue became empty, start again from the beginning
        rear = -1;
        RecHelp::addSlots(KIND_SLOTS, arr, MAX, usedRange(front, rear, MAX), "The queue is empty, so front and rear start again from the beginning.", 2, markers(front, rear), RecHelp::Hi());
    }
    return true;
}

bool ArrayQueue::peekFront(int& value) const {
    if (isEmpty()) {
        return false;
    }
    value = arr[front];
    return true;
}

void ArrayQueue::clear() {
    front = 0;
    rear = -1;
}

void ArrayQueue::display() const {
    if (isEmpty()) {
        cout << "Queue is empty." << endl;
        return;
    }
    cout << "Front -> ";
    for (int i = front; i <= rear; i++) {
        cout << arr[i] << " ";
    }
    cout << "<- Rear   (front index " << front << ", rear index " << rear << ")" << endl;
}
