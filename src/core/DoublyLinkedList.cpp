#include "DoublyLinkedList.h"
#include "RecordHelp.h"
#include <iostream>
using namespace std;

// ---------- recording helpers (used only when the recorder is switched on) ----------

namespace {
    const vector<string> CODE_HEAD = {
        "DNode* n = new DNode(v);",
        "n->next = head;  head->prev = n;",
        "head = n;"
    };
    const vector<string> CODE_TAIL = {
        "DNode* n = new DNode(v);",
        "n->prev = tail;  tail->next = n;",
        "tail = n;      // the tail pointer saves the walk"
    };
    const vector<string> CODE_POSITION = {
        "if (pos < 1 || pos > count + 1) return false;",
        "DNode* t = head;",
        "for (i = 1; i < pos - 1; i++) t = t->next;",
        "n->next = t->next;  n->prev = t;",
        "t->next->prev = n;  t->next = n;"
    };
    const vector<string> CODE_DELETE_FIRST = {
        "if (head == NULL) return false;",
        "DNode* t = head;",
        "head = head->next;  head->prev = NULL;",
        "delete t;"
    };
    const vector<string> CODE_DELETE_LAST = {
        "if (tail == NULL) return false;",
        "DNode* t = tail;",
        "tail = tail->prev;  tail->next = NULL;",
        "delete t;"
    };
    const vector<string> CODE_DELETE_VALUE = {
        "DNode* t = head;",
        "while (t && t->data != key) t = t->next;",
        "if (t == NULL) return false;",
        "t->prev->next = t->next;",
        "t->next->prev = t->prev;  delete t;"
    };
    const vector<string> CODE_DELETE_POSITION = {
        "if (pos < 1 || pos > count) return false;",
        "DNode* t = head;",
        "for (i = 1; i < pos; i++) t = t->next;",
        "t->prev->next = t->next;",
        "t->next->prev = t->prev;  delete t;"
    };
    const vector<string> CODE_SEARCH = {
        "DNode* t = head;  pos = 1;",
        "while (t != NULL) {",
        "  if (t->data == key) return pos;",
        "  t = t->next;  pos++; }",
        "return -1;"
    };
    const vector<string> CODE_SEARCH_END = {
        "DNode* t = tail;  pos = 1;",
        "while (t != NULL) {",
        "  if (t->data == key) return pos;",
        "  t = t->prev;  pos++; }",
        "return -1;"
    };
    const vector<string> CODE_REVERSE = {
        "DNode* cur = head;",
        "while (cur != NULL) {",
        "  swap(cur->next, cur->prev);",
        "  cur = cur->prev; }      // the old next",
        "swap(head, tail);"
    };

    vector<int> valuesOf(DNode* head) {
        vector<int> v;
        for (DNode* t = head; t != nullptr; t = t->next) v.push_back(t->data);
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

    void rec(DNode* head, const string& msg, int line, const RecHelp::Hi& hi = RecHelp::Hi(), bool floating = false, int floatValue = 0, int tIndex = -1) {
        if (!Recorder::enabled()) return;
        vector<int> v = valuesOf(head);
        vector<Marker> m = headTail(v.size());
        if (tIndex >= 0 && tIndex < (int)v.size()) m.push_back(Marker(tIndex, "t"));
        RecHelp::addValues(KIND_LIST, v, msg, line, m, hi, floating, floatValue, true, false);
    }
}

DoublyLinkedList::DoublyLinkedList() {
    head = nullptr;
    tail = nullptr;
    count = 0;
}

DoublyLinkedList::~DoublyLinkedList() {
    clear();
}

bool DoublyLinkedList::isEmpty() const {
    return head == nullptr;
}

int DoublyLinkedList::size() const {
    return count;
}

void DoublyLinkedList::insertAtBeginning(int data) {
    Recorder::begin("Insert at beginning (doubly)", CODE_HEAD);
    rec(head, "Create a new node holding " + to_string(data) + ".", 0, RecHelp::Hi(), true, data);
    DNode* newNode = new DNode(data);
    if (head == nullptr) {
        head = newNode; // first node is both head and tail
        tail = newNode;
    }
    else {
        rec(head, "The new node's next takes the old head, and the old head's prev points back to the new node.", 1, RecHelp::Hi(), true, data);
        newNode->next = head;
        head->prev = newNode;
        head = newNode;
    }
    count++;
    rec(head, "head now points to the new node. O(1).", 2, RecHelp::Hi(0, ROLE_NEW));
}

void DoublyLinkedList::insertAtEnd(int data) {
    Recorder::begin("Insert at end (doubly)", CODE_TAIL);
    rec(head, "Create a new node holding " + to_string(data) + ".", 0, RecHelp::Hi(), true, data);
    DNode* newNode = new DNode(data);
    if (tail == nullptr) {
        head = newNode;
        tail = newNode;
    }
    else {
        rec(head, "The tail pointer gives us the last node at once. Link it and the new node in both directions.", 1, RecHelp::Hi(), true, data);
        newNode->prev = tail; // the tail pointer saves us from walking the whole list
        tail->next = newNode;
        tail = newNode;
    }
    count++;
    rec(head, "tail now points to the new node. No walking needed: O(1).", 2, RecHelp::Hi(count - 1, ROLE_NEW));
}

bool DoublyLinkedList::insertAtPosition(int position, int data) {
    if (position < 1 || position > count + 1) {
        Recorder::begin("Insert at position (doubly)", CODE_POSITION);
        rec(head, "Position " + to_string(position) + " does not exist (valid: 1 to " + to_string(count + 1) + ").", 0);
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
    Recorder::begin("Insert at position (doubly)", CODE_POSITION);
    DNode* temp = head;
    rec(head, "Start at the head and walk to the node just before position " + to_string(position) + ".", 1, RecHelp::Hi(0, ROLE_CURRENT), false, 0, 0);
    for (int i = 1; i < position - 1; i++) {
        temp = temp->next; // node just before the new position
        rec(head, "Move t to the next node (" + to_string(temp->data) + ").", 2, RecHelp::Hi(i, ROLE_CURRENT), false, 0, i);
    }
    rec(head, "Create the new node " + to_string(data) + " and link it to t and t's next.", 3, RecHelp::Hi(position - 2, ROLE_CURRENT), true, data, position - 2);
    DNode* newNode = new DNode(data);
    newNode->next = temp->next;
    newNode->prev = temp;
    temp->next->prev = newNode;
    temp->next = newNode;
    count++;
    rec(head, "Both neighbours now point to the new node (4 pointers changed).", 4, RecHelp::Hi(position - 1, ROLE_NEW));
    return true;
}

bool DoublyLinkedList::deleteFirst() {
    Recorder::begin("Delete first (doubly)", CODE_DELETE_FIRST);
    if (head == nullptr) {
        rec(head, "The list is empty. Nothing to delete.", 0);
        return false;
    }
    rec(head, "Remember the first node in a temporary pointer.", 1, RecHelp::Hi(0, ROLE_CURRENT));
    DNode* temp = head;
    head = head->next;
    if (head != nullptr) {
        head->prev = nullptr;
    }
    else {
        tail = nullptr; // the list became empty
    }
    // show the old list while the node is still alive
    if (Recorder::enabled()) {
        vector<int> v = valuesOf(head);
        v.insert(v.begin(), temp->data);
        RecHelp::addValues(KIND_LIST, v, "head moves to the next node and its prev becomes NULL. The old first node is about to be freed.", 2, headTail(v.size()), RecHelp::Hi(0, ROLE_REMOVED), false, 0, true, false);
    }
    delete temp;
    count--;
    rec(head, "Node deleted. O(1).", 3);
    return true;
}

bool DoublyLinkedList::deleteLast() {
    Recorder::begin("Delete last (doubly)", CODE_DELETE_LAST);
    if (tail == nullptr) {
        rec(head, "The list is empty. Nothing to delete.", 0);
        return false;
    }
    rec(head, "The tail pointer gives the last node at once.", 1, RecHelp::Hi(count - 1, ROLE_CURRENT));
    DNode* temp = tail;
    tail = tail->prev;
    if (tail != nullptr) {
        tail->next = nullptr;
    }
    else {
        head = nullptr; // the list became empty
    }
    if (Recorder::enabled()) {
        vector<int> v = valuesOf(head);
        v.push_back(temp->data);
        RecHelp::addValues(KIND_LIST, v, "tail moves back along prev and its next becomes NULL. The old last node is about to be freed.", 2, headTail(v.size()), RecHelp::Hi((int)v.size() - 1, ROLE_REMOVED), false, 0, true, false);
    }
    delete temp;
    count--;
    rec(head, "Node deleted. No walk was needed: O(1).", 3);
    return true;
}

bool DoublyLinkedList::deleteByValue(int key) {
    Recorder::begin("Delete by value (doubly)", CODE_DELETE_VALUE);
    DNode* temp = head;
    int index = 0;
    rec(head, "Start at the head and look for " + to_string(key) + ".", 0, RecHelp::Hi(0, ROLE_CURRENT), false, 0, 0);
    while (temp != nullptr && temp->data != key) {
        temp = temp->next;
        index++;
        if (temp != nullptr) rec(head, "Not it. Move t to the next node (" + to_string(temp->data) + ").", 1, RecHelp::Hi(index, ROLE_CURRENT), false, 0, index);
    }
    if (temp == nullptr) {
        rec(head, to_string(key) + " is not in the list.", 2);
        return false; // key is not in the list
    }
    rec(head, "Found " + to_string(key) + ".", 1, RecHelp::Hi(index, ROLE_FOUND), false, 0, index);
    if (temp == head) {
        return deleteFirst();
    }
    if (temp == tail) {
        return deleteLast();
    }
    rec(head, "The previous and the next node are linked to each other (both directions), skipping this node.", 3, RecHelp::Hi(index, ROLE_REMOVED), false, 0, index);
    temp->prev->next = temp->next; // skip over the node in both directions
    temp->next->prev = temp->prev;
    delete temp;
    count--;
    rec(head, "Node deleted.", 4);
    return true;
}

bool DoublyLinkedList::deleteAtPosition(int position) {
    if (position < 1 || position > count) {
        Recorder::begin("Delete at position (doubly)", CODE_DELETE_POSITION);
        rec(head, "Position " + to_string(position) + " does not exist (valid: 1 to " + to_string(count) + ").", 0);
        return false;
    }
    if (position == 1) {
        return deleteFirst();
    }
    if (position == count) {
        return deleteLast();
    }
    Recorder::begin("Delete at position (doubly)", CODE_DELETE_POSITION);
    DNode* temp = head;
    rec(head, "Start at the head and walk to position " + to_string(position) + ".", 1, RecHelp::Hi(0, ROLE_CURRENT), false, 0, 0);
    for (int i = 1; i < position; i++) {
        temp = temp->next;
        rec(head, "Move t to the next node (" + to_string(temp->data) + ").", 2, RecHelp::Hi(i, ROLE_CURRENT), false, 0, i);
    }
    rec(head, "Link the previous and the next node to each other, skipping this node.", 3, RecHelp::Hi(position - 1, ROLE_REMOVED), false, 0, position - 1);
    temp->prev->next = temp->next;
    temp->next->prev = temp->prev;
    delete temp;
    count--;
    rec(head, "Node deleted.", 4);
    return true;
}

int DoublyLinkedList::search(int key) const {
    Recorder::begin("Search from the head (doubly)", CODE_SEARCH);
    DNode* temp = head;
    int position = 1;
    while (temp != nullptr) {
        if (temp->data == key) {
            rec(head, "Found " + to_string(key) + " at position " + to_string(position) + ".", 2, RecHelp::Hi(position - 1, ROLE_FOUND), false, 0, position - 1);
            return position;
        }
        rec(head, to_string(temp->data) + " is not " + to_string(key) + ". Move on.", 3, RecHelp::Hi(position - 1, ROLE_CURRENT), false, 0, position - 1);
        temp = temp->next;
        position++;
    }
    rec(head, "Reached NULL. " + to_string(key) + " is not in the list.", 4);
    return -1;
}

int DoublyLinkedList::searchFromEnd(int key) const {
    Recorder::begin("Search from the tail (doubly)", CODE_SEARCH_END);
    DNode* temp = tail;
    int position = 1;
    int index = count - 1;
    while (temp != nullptr) {
        if (temp->data == key) {
            rec(head, "Found " + to_string(key) + " at position " + to_string(position) + " counted from the end.", 2, RecHelp::Hi(index, ROLE_FOUND), false, 0, index);
            return position;
        }
        rec(head, to_string(temp->data) + " is not " + to_string(key) + ". Walk backwards with prev.", 3, RecHelp::Hi(index, ROLE_CURRENT), false, 0, index);
        temp = temp->prev;
        position++;
        index--;
    }
    rec(head, "Reached NULL. " + to_string(key) + " is not in the list.", 4);
    return -1;
}

void DoublyLinkedList::reverse() {
    Recorder::begin("Reverse (doubly)", CODE_REVERSE);
    vector<int> orig = valuesOf(head); // the real links are changed while we walk, so the pictures use this copy
    auto recOrig = [&](const string& msg, int line, int idx) {
        if (!Recorder::enabled()) return;
        vector<Marker> m = headTail(orig.size());
        m.push_back(Marker(idx, "cur"));
        RecHelp::addValues(KIND_LIST, orig, msg, line, m, RecHelp::Hi(idx, ROLE_CURRENT), false, 0, true, false);
    };
    if (!orig.empty()) recOrig("Walk through the list and swap the next and prev pointer of every node.", 0, 0);
    int index = 0;
    DNode* current = head;
    while (current != nullptr) {
        recOrig("Swap next and prev of " + to_string(current->data) + ".", 2, index);
        DNode* temp = current->next; // swap the two links of every node
        current->next = current->prev;
        current->prev = temp;
        current = temp;
        index++;
    }
    DNode* t = head; // head and tail trade places
    head = tail;
    tail = t;
    if (Recorder::enabled()) {
        vector<int> v = valuesOf(head);
        vector<Role> roles;
        RecHelp::Hi none;
        Frame f = RecHelp::valuesFrame(KIND_LIST, v, "Head and tail trade places. The list is reversed.", 4, headTail(v.size()), none);
        for (size_t i = 0; i < f.cells.size(); i++) f.cells[i].role = ROLE_DONE;
        f.doubly = true;
        Recorder::add(f);
    }
}

void DoublyLinkedList::clear() {
    while (head != nullptr) { // give every node back without recording anything
        DNode* temp = head;
        head = head->next;
        delete temp;
    }
    tail = nullptr;
    count = 0;
}

void DoublyLinkedList::displayForward() const {
    if (head == nullptr) {
        cout << "List is empty." << endl;
        return;
    }
    cout << "NULL <-> ";
    DNode* temp = head;
    while (temp != nullptr) {
        cout << temp->data << " <-> ";
        temp = temp->next;
    }
    cout << "NULL" << endl;
}

void DoublyLinkedList::displayBackward() const {
    if (tail == nullptr) {
        cout << "List is empty." << endl;
        return;
    }
    cout << "NULL <-> ";
    DNode* temp = tail;
    while (temp != nullptr) {
        cout << temp->data << " <-> ";
        temp = temp->prev;
    }
    cout << "NULL" << endl;
}
