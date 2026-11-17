#include "CircularLinkedList.h"
#include "RecordHelp.h"
#include <iostream>
using namespace std;

// ---------- recording helpers (used only when the recorder is switched on) ----------

namespace {
    const vector<string> CODE_HEAD = {
        "CNode* n = new CNode(v);",
        "n->next = tail->next;     // the old first node",
        "tail->next = n;           // the last node points to the new first node"
    };
    const vector<string> CODE_TAIL = {
        "CNode* n = new CNode(v);",
        "n->next = tail->next;     // points to the first node",
        "tail->next = n;",
        "tail = n;                 // the new node is the last one"
    };
    const vector<string> CODE_POSITION = {
        "if (pos < 1 || pos > count + 1) return false;",
        "CNode* t = tail->next;    // the first node",
        "for (i = 1; i < pos - 1; i++) t = t->next;",
        "n->next = t->next;",
        "t->next = n;"
    };
    const vector<string> CODE_DELETE_FIRST = {
        "if (tail == NULL) return false;",
        "CNode* first = tail->next;",
        "tail->next = first->next;   // the last node skips the first",
        "delete first;"
    };
    const vector<string> CODE_DELETE_LAST = {
        "if (tail == NULL) return false;",
        "CNode* t = tail->next;",
        "while (t->next != tail) t = t->next;   // no prev pointer: walk",
        "t->next = tail->next;  delete tail;  tail = t;"
    };
    const vector<string> CODE_DELETE_VALUE = {
        "CNode *prev = tail, *cur = tail->next;",
        "for (i = 0; i < count; i++) {       // the list never ends",
        "  if (cur->data == key) { prev->next = cur->next; delete cur; return true; }",
        "  prev = cur;  cur = cur->next; }",
        "return false;"
    };
    const vector<string> CODE_DELETE_POSITION = {
        "if (pos < 1 || pos > count) return false;",
        "CNode* prev = tail->next;",
        "for (i = 1; i < pos - 1; i++) prev = prev->next;",
        "CNode* t = prev->next;",
        "prev->next = t->next;  delete t;"
    };
    const vector<string> CODE_SEARCH = {
        "CNode* t = tail->next;",
        "for (pos = 1; pos <= count; pos++) {   // stop after count nodes",
        "  if (t->data == key) return pos;",
        "  t = t->next; }",
        "return -1;"
    };

    vector<int> valuesOf(CNode* tail, int count) {
        vector<int> v;
        if (tail == nullptr) return v;
        CNode* t = tail->next;
        for (int i = 0; i < count; i++) {
            v.push_back(t->data);
            t = t->next;
        }
        return v;
    }

    vector<Marker> headTail(size_t n) {
        vector<Marker> m;
        if (n == 1) {
            m.push_back(Marker(0, "head / tail"));
        }
        else if (n > 1) {
            m.push_back(Marker(0, "head"));
            m.push_back(Marker((int)n - 1, "tail"));
        }
        return m;
    }

    void rec(CNode* tail, int count, const string& msg, int line, const RecHelp::Hi& hi = RecHelp::Hi(), bool floating = false, int floatValue = 0, int tIndex = -1) {
        if (!Recorder::enabled()) return;
        vector<int> v = valuesOf(tail, count);
        vector<Marker> m = headTail(v.size());
        if (tIndex >= 0 && tIndex < (int)v.size()) m.push_back(Marker(tIndex, "t"));
        RecHelp::addValues(KIND_LIST, v, msg, line, m, hi, floating, floatValue, false, true);
    }
}

CircularLinkedList::CircularLinkedList() {
    tail = nullptr;
    count = 0;
}

CircularLinkedList::~CircularLinkedList() {
    clear();
}

bool CircularLinkedList::isEmpty() const {
    return tail == nullptr;
}

int CircularLinkedList::size() const {
    return count;
}

void CircularLinkedList::insertAtBeginning(int data) {
    Recorder::begin("Insert at beginning (circular)", CODE_HEAD);
    rec(tail, count, "Create a new node holding " + to_string(data) + ".", 0, RecHelp::Hi(), true, data);
    CNode* newNode = new CNode(data);
    if (tail == nullptr) {
        tail = newNode;
        newNode->next = newNode; // a single node points to itself
    }
    else {
        rec(tail, count, "The new node points to the old first node (tail->next).", 1, RecHelp::Hi(), true, data);
        newNode->next = tail->next; // new node points to the old first node
        tail->next = newNode;       // the last node points to the new first node
    }
    count++;
    rec(tail, count, "The last node now points to the new first node. The circle is closed. O(1).", 2, RecHelp::Hi(0, ROLE_NEW));
}

void CircularLinkedList::insertAtEnd(int data) {
    Recorder::begin("Insert at end (circular)", CODE_TAIL);
    rec(tail, count, "Create a new node holding " + to_string(data) + ".", 0, RecHelp::Hi(), true, data);
    CNode* newNode = new CNode(data);
    if (tail == nullptr) {
        tail = newNode;
        newNode->next = newNode;
    }
    else {
        rec(tail, count, "The new node points to the first node, so it will close the circle.", 1, RecHelp::Hi(), true, data);
        newNode->next = tail->next;
        tail->next = newNode;
        tail = newNode;
    }
    count++;
    rec(tail, count, "tail moves to the new node. No walking needed: O(1).", 3, RecHelp::Hi(count - 1, ROLE_NEW));
}

bool CircularLinkedList::insertAtPosition(int position, int data) {
    if (position < 1 || position > count + 1) {
        Recorder::begin("Insert at position (circular)", CODE_POSITION);
        rec(tail, count, "Position " + to_string(position) + " does not exist (valid: 1 to " + to_string(count + 1) + ").", 0);
        return false;
    }
    if (position == 1) {
        insertAtBeginning(data);
        return true;
    }
    if (position == count + 1) {
        insertAtEnd(data);
        return true;
    }
    Recorder::begin("Insert at position (circular)", CODE_POSITION);
    CNode* temp = tail->next; // start at the first node
    rec(tail, count, "Start at the first node and walk to the node just before position " + to_string(position) + ".", 1, RecHelp::Hi(0, ROLE_CURRENT), false, 0, 0);
    for (int i = 1; i < position - 1; i++) {
        temp = temp->next;    // node just before the new position
        rec(tail, count, "Move t to the next node (" + to_string(temp->data) + ").", 2, RecHelp::Hi(i, ROLE_CURRENT), false, 0, i);
    }
    rec(tail, count, "Create the new node " + to_string(data) + " and let it point to t's next.", 3, RecHelp::Hi(position - 2, ROLE_CURRENT), true, data, position - 2);
    CNode* newNode = new CNode(data);
    newNode->next = temp->next;
    temp->next = newNode;
    count++;
    rec(tail, count, "t now points to the new node.", 4, RecHelp::Hi(position - 1, ROLE_NEW));
    return true;
}

bool CircularLinkedList::deleteFirst() {
    Recorder::begin("Delete first (circular)", CODE_DELETE_FIRST);
    if (tail == nullptr) {
        rec(tail, count, "The list is empty. Nothing to delete.", 0);
        return false;
    }
    rec(tail, count, "The first node is tail->next.", 1, RecHelp::Hi(0, ROLE_CURRENT));
    CNode* first = tail->next;
    vector<int> before = valuesOf(tail, count);
    if (first == tail) { // only one node
        tail = nullptr;
    }
    else {
        tail->next = first->next;
    }
    RecHelp::addValues(KIND_LIST, before, "The last node now skips the first node and points to the second. The old first node is about to be freed.", 2, headTail(before.size()), RecHelp::Hi(0, ROLE_REMOVED), false, 0, false, true);
    delete first;
    count--;
    rec(tail, count, "Node deleted. O(1).", 3);
    return true;
}

bool CircularLinkedList::deleteLast() {
    Recorder::begin("Delete last (circular)", CODE_DELETE_LAST);
    if (tail == nullptr) {
        rec(tail, count, "The list is empty. Nothing to delete.", 0);
        return false;
    }
    if (tail->next == tail) { // only one node
        rec(tail, count, "Only one node: it points to itself. Delete it.", 3, RecHelp::Hi(0, ROLE_REMOVED));
        delete tail;
        tail = nullptr;
        count--;
        rec(tail, count, "The list is empty now.", 3);
        return true;
    }
    CNode* temp = tail->next;
    int index = 0;
    rec(tail, count, "There is no prev pointer, so walk from the first node to the node before the tail.", 2, RecHelp::Hi(0, ROLE_CURRENT), false, 0, 0);
    while (temp->next != tail) {
        temp = temp->next; // we have no prev pointer, so walk to the node before the tail
        index++;
        rec(tail, count, "Move t to the next node (" + to_string(temp->data) + ").", 2, RecHelp::Hi(index, ROLE_CURRENT), false, 0, index);
    }
    vector<int> before = valuesOf(tail, count);
    temp->next = tail->next;
    RecHelp::addValues(KIND_LIST, before, "t links to the first node and becomes the new tail. The old last node is about to be freed.", 3, headTail(before.size()), RecHelp::Hi((int)before.size() - 1, ROLE_REMOVED), false, 0, false, true);
    delete tail;
    tail = temp;
    count--;
    rec(tail, count, "Node deleted. Finding the node before the tail cost O(n).", 3);
    return true;
}

bool CircularLinkedList::deleteByValue(int key) {
    Recorder::begin("Delete by value (circular)", CODE_DELETE_VALUE);
    if (tail == nullptr) {
        rec(tail, count, "The list is empty.", 0);
        return false;
    }
    CNode* prev = tail;
    CNode* current = tail->next;
    for (int i = 0; i < count; i++) { // the list never ends, so we stop after count nodes
        if (current->data == key) {
            rec(tail, count, "Found " + to_string(key) + ".", 2, RecHelp::Hi(i, ROLE_FOUND), false, 0, i);
            if (current == tail->next) {
                return deleteFirst();
            }
            if (current == tail) {
                return deleteLast();
            }
            rec(tail, count, "The previous node skips this node.", 2, RecHelp::Hi(i, ROLE_REMOVED), false, 0, i);
            prev->next = current->next;
            delete current;
            count--;
            rec(tail, count, "Node deleted.", 2);
            return true;
        }
        rec(tail, count, to_string(current->data) + " is not " + to_string(key) + ". Move on.", 3, RecHelp::Hi(i, ROLE_CURRENT), false, 0, i);
        prev = current;
        current = current->next;
    }
    rec(tail, count, "Went around the whole circle (" + to_string(count) + " nodes). " + to_string(key) + " is not in the list.", 4);
    return false;
}

bool CircularLinkedList::deleteAtPosition(int position) {
    if (position < 1 || position > count) {
        Recorder::begin("Delete at position (circular)", CODE_DELETE_POSITION);
        rec(tail, count, "Position " + to_string(position) + " does not exist (valid: 1 to " + to_string(count) + ").", 0);
        return false;
    }
    if (position == 1) {
        return deleteFirst();
    }
    if (position == count) {
        return deleteLast();
    }
    Recorder::begin("Delete at position (circular)", CODE_DELETE_POSITION);
    CNode* prev = tail->next;
    rec(tail, count, "Walk to the node before position " + to_string(position) + ".", 1, RecHelp::Hi(0, ROLE_CURRENT), false, 0, 0);
    for (int i = 1; i < position - 1; i++) {
        prev = prev->next; // node before the one to delete
        rec(tail, count, "Move t to the next node (" + to_string(prev->data) + ").", 2, RecHelp::Hi(i, ROLE_CURRENT), false, 0, i);
    }
    CNode* temp = prev->next;
    rec(tail, count, "The node after t is the one to delete.", 3, RecHelp::Hi(position - 1, ROLE_REMOVED), false, 0, position - 2);
    prev->next = temp->next;
    delete temp;
    count--;
    rec(tail, count, "Node deleted.", 4);
    return true;
}

int CircularLinkedList::search(int key) const {
    Recorder::begin("Search (circular)", CODE_SEARCH);
    if (tail == nullptr) {
        rec(tail, count, "The list is empty.", 0);
        return -1;
    }
    CNode* temp = tail->next;
    for (int position = 1; position <= count; position++) {
        if (temp->data == key) {
            rec(tail, count, "Found " + to_string(key) + " at position " + to_string(position) + ".", 2, RecHelp::Hi(position - 1, ROLE_FOUND), false, 0, position - 1);
            return position;
        }
        rec(tail, count, to_string(temp->data) + " is not " + to_string(key) + ". Move on.", 3, RecHelp::Hi(position - 1, ROLE_CURRENT), false, 0, position - 1);
        temp = temp->next;
    }
    rec(tail, count, "Checked all " + to_string(count) + " nodes without going around forever. " + to_string(key) + " is not in the list.", 4);
    return -1;
}

bool CircularLinkedList::getFirst(int& value) const {
    if (tail == nullptr) {
        return false;
    }
    value = tail->next->data;
    return true;
}

bool CircularLinkedList::getLast(int& value) const {
    if (tail == nullptr) {
        return false;
    }
    value = tail->data;
    return true;
}

bool CircularLinkedList::getNextOfLast(int& value) const {
    if (tail == nullptr) {
        return false;
    }
    value = tail->next->data; // follows the circular link from the last node
    return true;
}

void CircularLinkedList::clear() {
    while (tail != nullptr) { // give every node back without recording anything
        CNode* first = tail->next;
        if (first == tail) {
            tail = nullptr;
        }
        else {
            tail->next = first->next;
        }
        delete first;
    }
    count = 0;
}

void CircularLinkedList::display() const {
    if (tail == nullptr) {
        cout << "List is empty." << endl;
        return;
    }
    CNode* temp = tail->next;
    for (int i = 0; i < count; i++) {
        cout << temp->data << " -> ";
        temp = temp->next;
    }
    cout << "(back to " << tail->next->data << ")" << endl;
}
