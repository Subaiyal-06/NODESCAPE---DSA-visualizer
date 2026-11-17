#include "LinkedStack.h"
#include "Trace.h"
#include "RecordHelp.h"
#include <iostream>
using namespace std;

namespace {
    const vector<string> CODE_PUSH = {
        "Node* n = new Node(value);",
        "n->next = top;",
        "top = n;"
    };
    const vector<string> CODE_POP = {
        "if (top == NULL) return false;      // underflow",
        "Node* t = top; value = t->data;",
        "top = top->next;",
        "delete t;"
    };

    // values from the bottom of the stack to the top
    vector<int> bottomToTop(StackNode* top) {
        vector<int> v;
        for (StackNode* t = top; t != nullptr; t = t->next) v.push_back(t->data);
        return vector<int>(v.rbegin(), v.rend());
    }

    vector<Marker> topMarker(const vector<int>& v) {
        vector<Marker> m;
        if (!v.empty()) m.push_back(Marker((int)v.size() - 1, "top"));
        return m;
    }
}

LinkedStack::LinkedStack() {
    top = nullptr;
    count = 0;
}

LinkedStack::~LinkedStack() {
    clear();
}

bool LinkedStack::isEmpty() const {
    return top == nullptr;
}

int LinkedStack::size() const {
    return count;
}

void LinkedStack::push(int value) {
    Recorder::begin("Push (linked stack)", CODE_PUSH);
    if (Recorder::enabled()) {
        vector<int> before = bottomToTop(top);
        RecHelp::addValues(KIND_STACK, before, "Create a new node holding " + to_string(value) + ".", 0, topMarker(before), RecHelp::Hi(), true, value);
        RecHelp::addValues(KIND_STACK, before, "The new node's next pointer takes the old top.", 1, topMarker(before), RecHelp::Hi(), true, value);
    }
    StackNode* newNode = new StackNode(value);
    newNode->next = top;
    top = newNode;
    count++;
    Trace::say("new node " + to_string(value) + " placed on top");
    if (Recorder::enabled()) {
        vector<int> after = bottomToTop(top);
        RecHelp::addValues(KIND_STACK, after, "top now points to the new node. No shifting and no size limit: O(1).", 2, topMarker(after), RecHelp::Hi((int)after.size() - 1, ROLE_NEW));
    }
}

bool LinkedStack::pop(int& value) {
    Recorder::begin("Pop (linked stack)", CODE_POP);
    if (top == nullptr) {
        Trace::say("top is NULL, stack underflow");
        RecHelp::addValues(KIND_STACK, vector<int>(), "top is NULL: the stack is empty. Underflow, nothing to pop.", 0, vector<Marker>(), RecHelp::Hi());
        return false;
    }
    vector<int> before = bottomToTop(top);
    RecHelp::addValues(KIND_STACK, before, "Remember the top node " + to_string(top->data) + " in a temporary pointer.", 1, topMarker(before), RecHelp::Hi((int)before.size() - 1, ROLE_CURRENT));
    StackNode* temp = top;
    value = temp->data;
    top = top->next;
    RecHelp::addValues(KIND_STACK, before, "top moves to the next node. The old top node is about to be freed.", 2, topMarker(before), RecHelp::Hi((int)before.size() - 1, ROLE_REMOVED));
    delete temp;
    count--;
    Trace::say("removed " + to_string(value) + " from the top");
    vector<int> after = bottomToTop(top);
    RecHelp::addValues(KIND_STACK, after, "Node deleted. " + to_string(value) + " is returned.", 3, topMarker(after), RecHelp::Hi());
    return true;
}

bool LinkedStack::peek(int& value) const {
    if (top == nullptr) {
        return false;
    }
    value = top->data;
    return true;
}

void LinkedStack::clear() {
    while (top != nullptr) { // give every node back without recording anything
        StackNode* temp = top;
        top = top->next;
        delete temp;
    }
    count = 0;
}

void LinkedStack::display() const {
    if (top == nullptr) {
        cout << "Stack is empty." << endl;
        return;
    }
    cout << "Top -> ";
    StackNode* temp = top;
    while (temp != nullptr) {
        cout << temp->data << " -> ";
        temp = temp->next;
    }
    cout << "NULL" << endl;
}
