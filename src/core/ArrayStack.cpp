#include "ArrayStack.h"
#include "Trace.h"
#include "RecordHelp.h"
#include <iostream>
using namespace std;

namespace {
    const vector<string> CODE_PUSH = {
        "if (top == MAX - 1) return false;   // overflow",
        "top++;",
        "arr[top] = value;"
    };
    const vector<string> CODE_POP = {
        "if (top == -1) return false;        // underflow",
        "value = arr[top];",
        "top--;"
    };

    vector<char> usedUpTo(int top, int cap) {
        vector<char> used(cap, 0);
        for (int i = 0; i <= top && i < cap; i++) used[i] = 1;
        return used;
    }

    vector<Marker> topMarker(int top) {
        vector<Marker> m;
        if (top >= 0) m.push_back(Marker(top, "top"));
        return m;
    }
}

ArrayStack::ArrayStack() {
    top = -1;
}

bool ArrayStack::isEmpty() const {
    return top == -1;
}

bool ArrayStack::isFull() const {
    return top == MAX - 1;
}

int ArrayStack::size() const {
    return top + 1;
}

bool ArrayStack::push(int value) {
    Recorder::begin("Push (array stack)", CODE_PUSH);
    if (isFull()) {
        Trace::say("top = MAX - 1, stack overflow");
        RecHelp::addSlots(KIND_STACK, arr, MAX, usedUpTo(top, MAX), "The stack is full (top = " + to_string(top) + " = MAX - 1). Overflow: nothing is pushed.", 0, topMarker(top), RecHelp::Hi(top, ROLE_KEPT));
        return false;
    }
    top++;
    RecHelp::addSlots(KIND_STACK, arr, MAX, usedUpTo(top - 1, MAX), "top moves up by one, to index " + to_string(top) + ".", 1, topMarker(top), RecHelp::Hi(top, ROLE_CURRENT));
    arr[top] = value;
    Trace::say("top moved to " + to_string(top) + ", stored " + to_string(value));
    RecHelp::addSlots(KIND_STACK, arr, MAX, usedUpTo(top, MAX), "Store " + to_string(value) + " in arr[" + to_string(top) + "]. O(1).", 2, topMarker(top), RecHelp::Hi(top, ROLE_NEW));
    return true;
}

bool ArrayStack::pop(int& value) {
    Recorder::begin("Pop (array stack)", CODE_POP);
    if (isEmpty()) {
        Trace::say("top = -1, stack underflow");
        RecHelp::addSlots(KIND_STACK, arr, MAX, usedUpTo(top, MAX), "The stack is empty (top = -1). Underflow: nothing to pop.", 0, topMarker(top), RecHelp::Hi());
        return false;
    }
    value = arr[top];
    RecHelp::addSlots(KIND_STACK, arr, MAX, usedUpTo(top, MAX), "The value on top is " + to_string(value) + ". It is returned.", 1, topMarker(top), RecHelp::Hi(top, ROLE_CURRENT));
    int old = top;
    top--;
    Trace::say("returned " + to_string(value) + ", top moved to " + to_string(top));
    RecHelp::addSlots(KIND_STACK, arr, MAX, usedUpTo(top, MAX), "top moves down to " + to_string(top) + ". The slot is free again (the old value is simply ignored).", 2, topMarker(top), RecHelp::Hi(old, ROLE_REMOVED));
    return true;
}

bool ArrayStack::peek(int& value) const {
    if (isEmpty()) {
        return false;
    }
    value = arr[top];
    return true;
}

void ArrayStack::clear() {
    top = -1;
}

void ArrayStack::display() const {
    if (isEmpty()) {
        cout << "Stack is empty." << endl;
        return;
    }
    cout << "Top -> ";
    for (int i = top; i >= 0; i--) {
        cout << arr[i] << " ";
    }
    cout << "<- Bottom" << endl;
}
