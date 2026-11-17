#include "LinkedList.h"
#include "Recorder.h"
#include <iostream>
#include <string>
#include <vector>
using namespace std;

// ---------- recording helpers (used only when the recorder is switched on) ----------

namespace {
    // which cells of the list are highlighted in a frame
    struct ListView {
        int cur;       // node being looked at
        int other;     // a second node being looked at
        int found;
        int added;
        int removed;
        int prev;      // node before cur
        bool floating; // a new node that is not linked yet
        int floatValue;
        long visited;

        ListView() : cur(-1), other(-1), found(-1), added(-1), removed(-1), prev(-1),
                     floating(false), floatValue(0), visited(0) {}
    };

    const vector<string> CODE_HEAD = {
        "Node* n = new Node(v);",
        "n->next = head;",
        "head = n;"
    };
    const vector<string> CODE_TAIL = {
        "Node* n = new Node(v);",
        "if (head == NULL) head = n;",
        "else { Node* t = head;",
        "  while (t->next != NULL) t = t->next;",
        "  t->next = n; }"
    };
    const vector<string> CODE_POSITION = {
        "if (pos < 1 || pos > count + 1) return false;",
        "if (pos == 1) { insertAtBeginning(v); return true; }",
        "Node* t = head;",
        "for (i = 1; i < pos - 1; i++) t = t->next;",
        "Node* n = new Node(v);",
        "n->next = t->next;",
        "t->next = n;"
    };
    const vector<string> CODE_DELETE_FIRST = {
        "if (head == NULL) return false;",
        "Node* t = head;",
        "head = head->next;",
        "delete t;"
    };
    const vector<string> CODE_DELETE_LAST = {
        "if (head == NULL) return false;",
        "if (head->next == NULL) { delete head; head = NULL; return true; }",
        "Node* t = head;",
        "while (t->next->next != NULL) t = t->next;",
        "delete t->next;",
        "t->next = NULL;"
    };
    const vector<string> CODE_DELETE_VALUE = {
        "if (head == NULL) return false;",
        "if (head->data == key) return deleteFirst();",
        "Node *prev = head, *t = head->next;",
        "while (t && t->data != key) { prev = t; t = t->next; }",
        "if (t == NULL) return false;",
        "prev->next = t->next;",
        "delete t;"
    };
    const vector<string> CODE_DELETE_POSITION = {
        "if (pos < 1 || pos > count) return false;",
        "if (pos == 1) return deleteFirst();",
        "Node* prev = head;",
        "for (i = 1; i < pos - 1; i++) prev = prev->next;",
        "Node* t = prev->next;",
        "prev->next = t->next;",
        "delete t;"
    };
    const vector<string> CODE_SEARCH = {
        "Node* t = head; int pos = 1;",
        "while (t != NULL) {",
        "  if (t->data == key) return pos;",
        "  t = t->next; pos++; }",
        "return -1;"
    };
    const vector<string> CODE_REVERSE = {
        "Node *prev = NULL, *cur = head;",
        "while (cur != NULL) {",
        "  Node* next = cur->next;",
        "  cur->next = prev;",
        "  prev = cur; cur = next; }",
        "head = prev;"
    };
    const vector<string> CODE_SORT = {
        "do { swapped = false;",
        "  for (t = head; t->next; t = t->next)",
        "    if (t->data > t->next->data) {",
        "      swap(t->data, t->next->data);",
        "      swapped = true; }",
        "} while (swapped);"
    };

    // takes one picture of the list
    void recordList(Node* head, const string& message, int line, const ListView& view) {
        if (!Recorder::enabled()) {
            return;
        }
        Frame f;
        f.kind = KIND_LIST;
        f.message = message;
        f.codeLine = line;
        f.comparisons = view.visited;

        int index = 0;
        for (Node* temp = head; temp != nullptr; temp = temp->next) {
            Role role = ROLE_NORMAL;
            if (index == view.cur || index == view.other) role = ROLE_CURRENT;
            if (index == view.found) role = ROLE_FOUND;
            if (index == view.added) role = ROLE_NEW;
            if (index == view.removed) role = ROLE_REMOVED;
            f.cells.push_back(Cell(temp->data, role));
            index++;
        }
        if (index > 0) {
            f.markers.push_back(Marker(0, "head"));
        }
        if (view.prev >= 0) {
            f.markers.push_back(Marker(view.prev, "prev"));
        }
        if (view.cur >= 0) {
            f.markers.push_back(Marker(view.cur, "t"));
        }
        f.hasFloating = view.floating;
        f.floating = Cell(view.floatValue, ROLE_NEW);
        Recorder::add(f);
    }
}

// ---------- the list itself ----------

LinkedList::LinkedList() {
    head = nullptr;
    count = 0;
}

LinkedList::~LinkedList() {
    clear(); // give every node back to the heap
}

bool LinkedList::isEmpty() const {
    return head == nullptr;
}

int LinkedList::size() const {
    return count;
}

void LinkedList::pushFront(int data) {
    Node* newNode = new Node(data);
    newNode->next = head; // new node points to the old first node
    head = newNode;       // head now points to the new node
    count++;
}

bool LinkedList::popFront() {
    if (head == nullptr) {
        return false;
    }
    Node* temp = head;
    head = head->next;
    delete temp;
    count--;
    return true;
}

void LinkedList::insertAtBeginning(int data) {
    Recorder::begin("Insert at beginning", CODE_HEAD);
    ListView v;
    v.floating = true;
    v.floatValue = data;
    recordList(head, "Create a new node holding " + to_string(data) + ".", 0, v);
    recordList(head, "The new node's next pointer takes the old head.", 1, v);

    pushFront(data);

    v.floating = false;
    v.added = 0;
    recordList(head, "Head now points to the new node. No walking was needed: O(1).", 2, v);
}

void LinkedList::insertAtEnd(int data) {
    Recorder::begin("Insert at end", CODE_TAIL);
    ListView v;
    v.floating = true;
    v.floatValue = data;
    recordList(head, "Create a new node holding " + to_string(data) + ".", 0, v);

    Node* newNode = new Node(data);

    if (head == nullptr) {
        recordList(head, "The list is empty, so head will point to the new node.", 1, v);
        head = newNode; // empty list: the new node is the whole list
        v.floating = false;
        v.added = 0;
        recordList(head, "The new node is now the whole list.", 1, v);
    }
    else {
        Node* temp = head;
        int index = 0;
        v.visited = 1;
        v.cur = 0;
        recordList(head, "Start at the first node.", 2, v);
        while (temp->next != nullptr) {
            temp = temp->next; // walk to the last node
            index++;
            v.cur = index;
            v.visited++;
            recordList(head, "Move to the next node (" + to_string(temp->data) + ").", 3, v);
        }
        temp->next = newNode;
        v.floating = false;
        v.added = index + 1;
        recordList(head, "The last node's next now points to the new node: O(n) steps were needed.", 4, v);
    }
    count++;
}

bool LinkedList::insertAtPosition(int position, int data) {
    Recorder::begin("Insert at position", CODE_POSITION);
    ListView v;
    // valid positions are 1 to count + 1 (count + 1 means "at the end")
    if (position < 1 || position > count + 1) {
        recordList(head, "Position " + to_string(position) + " is outside 1 to " + to_string(count + 1) + ". Nothing is inserted.", 0, v);
        return false;
    }
    if (position == 1) {
        v.floating = true;
        v.floatValue = data;
        recordList(head, "Position 1 is the beginning of the list.", 1, v);
        pushFront(data);
        v.floating = false;
        v.added = 0;
        recordList(head, "Inserted at the beginning.", 1, v);
        return true;
    }

    Node* temp = head;
    v.cur = 0;
    v.visited = 1;
    recordList(head, "Start at the first node.", 2, v);
    for (int i = 1; i < position - 1; i++) {
        temp = temp->next; // stop at the node just before the position
        v.cur = i;
        v.visited++;
        recordList(head, "Move to the next node (" + to_string(temp->data) + ").", 3, v);
    }
    v.floating = true;
    v.floatValue = data;
    recordList(head, "Stopped at the node before position " + to_string(position) + ". Create the new node.", 4, v);

    Node* newNode = new Node(data);
    newNode->next = temp->next;
    recordList(head, "The new node points to the node that follows.", 5, v);
    temp->next = newNode;
    count++;
    v.floating = false;
    v.cur = -1;
    v.added = position - 1;
    recordList(head, "The previous node now points to the new node.", 6, v);
    return true;
}

bool LinkedList::deleteFirst() {
    Recorder::begin("Delete first node", CODE_DELETE_FIRST);
    ListView v;
    if (head == nullptr) {
        recordList(head, "The list is empty. Nothing to delete.", 0, v);
        return false;
    }
    v.cur = 0;
    recordList(head, "t points to the first node.", 1, v);
    v.cur = -1;
    v.removed = 0;
    recordList(head, "Head will move to the second node.", 2, v);
    popFront();
    recordList(head, "The old first node is deleted: O(1).", 3, ListView());
    return true;
}

bool LinkedList::deleteLast() {
    Recorder::begin("Delete last node", CODE_DELETE_LAST);
    ListView v;
    if (head == nullptr) {
        recordList(head, "The list is empty. Nothing to delete.", 0, v);
        return false;
    }
    if (head->next == nullptr) { // only one node
        v.removed = 0;
        recordList(head, "There is only one node, so the list becomes empty.", 1, v);
        popFront();
        recordList(head, "The list is empty now.", 1, ListView());
        return true;
    }

    Node* temp = head;
    int index = 0;
    v.cur = 0;
    v.visited = 1;
    recordList(head, "Start at the first node.", 2, v);
    while (temp->next->next != nullptr) {
        temp = temp->next; // stop at the second last node
        index++;
        v.cur = index;
        v.visited++;
        recordList(head, "The node after the next one is not NULL, so move on.", 3, v);
    }
    v.removed = index + 1;
    recordList(head, "t is the second last node. The last node will be deleted.", 4, v);
    delete temp->next;
    temp->next = nullptr;
    count--;
    ListView done;
    done.cur = index;
    recordList(head, "t->next is set to NULL: O(n) steps were needed to get here.", 5, done);
    return true;
}

bool LinkedList::deleteByValue(int key) {
    Recorder::begin("Delete by value", CODE_DELETE_VALUE);
    ListView v;
    if (head == nullptr) {
        recordList(head, "The list is empty. Nothing to delete.", 0, v);
        return false;
    }
    v.cur = 0;
    v.visited = 1;
    if (head->data == key) {
        v.found = 0;
        recordList(head, "The first node holds " + to_string(key) + ".", 1, v);
        v.found = -1;
        v.removed = 0;
        recordList(head, "Head will move to the second node.", 1, v);
        popFront();
        recordList(head, "The first node is deleted.", 1, ListView());
        return true;
    }

    Node* prev = head;
    Node* temp = head->next;
    int index = 1;
    v.prev = 0;
    v.cur = 1;
    v.visited = 2;
    recordList(head, "prev is the first node, t is the second node.", 2, v);
    while (temp != nullptr && temp->data != key) {
        recordList(head, to_string(temp->data) + " is not " + to_string(key) + ". Move both pointers.", 3, v);
        prev = temp;
        temp = temp->next;
        index++;
        v.prev = index - 1;
        v.cur = index;
        v.visited++;
        if (temp == nullptr) {
            v.cur = -1;
        }
    }
    if (temp == nullptr) {
        recordList(head, "Reached NULL. " + to_string(key) + " is not in the list, nothing deleted.", 4, ListView());
        return false;
    }
    v.found = index;
    recordList(head, "Found " + to_string(key) + ".", 3, v);
    v.found = -1;
    v.removed = index;
    recordList(head, "prev will skip over the node.", 5, v);
    prev->next = temp->next; // unlink the node
    delete temp;
    count--;
    ListView done;
    done.cur = index - 1;
    recordList(head, "The node is deleted.", 6, done);
    return true;
}

bool LinkedList::deleteAtPosition(int position) {
    Recorder::begin("Delete at position", CODE_DELETE_POSITION);
    ListView v;
    if (position < 1 || position > count) {
        recordList(head, "Position " + to_string(position) + " is outside 1 to " + to_string(count) + ". Nothing is deleted.", 0, v);
        return false;
    }
    if (position == 1) {
        v.removed = 0;
        recordList(head, "Position 1 is the first node.", 1, v);
        popFront();
        recordList(head, "The first node is deleted.", 1, ListView());
        return true;
    }

    Node* prev = head;
    v.cur = 0;
    v.visited = 1;
    recordList(head, "Start at the first node.", 2, v);
    for (int i = 1; i < position - 1; i++) {
        prev = prev->next; // stop at the node before the one to delete
        v.cur = i;
        v.visited++;
        recordList(head, "Move to the next node.", 3, v);
    }
    v.removed = position - 1;
    recordList(head, "prev is the node before position " + to_string(position) + ". The next node will be deleted.", 4, v);
    Node* temp = prev->next;
    prev->next = temp->next;
    delete temp;
    count--;
    ListView done;
    done.cur = position - 2;
    recordList(head, "prev now skips the deleted node.", 5, done);
    return true;
}

int LinkedList::search(int key) const {
    Recorder::begin("Search", CODE_SEARCH);
    ListView v;
    Node* temp = head;
    int position = 1;
    while (temp != nullptr) {
        v.cur = position - 1;
        v.visited = position;
        if (temp->data == key) {
            v.cur = -1;
            v.found = position - 1;
            recordList(head, "Found " + to_string(key) + " at position " + to_string(position) + ".", 2, v);
            return position;
        }
        recordList(head, to_string(temp->data) + " is not " + to_string(key) + ".", 2, v);
        temp = temp->next;
        position++;
    }
    ListView end;
    end.visited = position - 1;
    recordList(head, "Reached NULL. " + to_string(key) + " is not in the list.", 4, end);
    return -1;
}

void LinkedList::reverse() {
    Recorder::begin("Reverse the list", CODE_REVERSE);
    ListView v;
    Node* prev = nullptr;
    Node* current = head;
    int index = 0;
    while (current != nullptr) {
        v.cur = index;
        v.prev = index - 1;
        v.visited = index + 1;
        recordList(head, "Turn the link of " + to_string(current->data) + " around so it points backward.", 3, v);
        Node* nextNode = current->next; // remember the rest of the list
        current->next = prev;           // turn the link around
        prev = current;
        current = nextNode;
        index++;
    }
    head = prev;
    ListView done;
    recordList(head, "Head now points to the old last node. The list is reversed: O(n).", 5, done);
}

void LinkedList::sortAscending() {
    Recorder::begin("Sort the list", CODE_SORT);
    if (count < 2) {
        recordList(head, "A list with fewer than two nodes is already sorted.", 0, ListView());
        return;
    }
    // Bubble sort that swaps the data of neighbouring nodes
    bool swapped = true;
    long comparisons = 0;
    while (swapped) {
        swapped = false;
        Node* temp = head;
        int index = 0;
        while (temp->next != nullptr) {
            comparisons++;
            ListView v;
            v.cur = index;
            v.other = index + 1;
            v.visited = comparisons;
            if (temp->data > temp->next->data) {
                recordList(head, to_string(temp->data) + " > " + to_string(temp->next->data) + ", so swap the two values.", 2, v);
                int t = temp->data;
                temp->data = temp->next->data;
                temp->next->data = t;
                swapped = true;
                v.cur = -1;
                v.other = -1;
                v.added = index;
                recordList(head, "Swapped.", 3, v);
            }
            else {
                recordList(head, to_string(temp->data) + " <= " + to_string(temp->next->data) + ", already in order.", 2, v);
            }
            temp = temp->next;
            index++;
        }
    }
    Frame f;
    f.kind = KIND_LIST;
    f.message = "No swaps in a whole pass: the list is sorted. " + to_string(comparisons) + " comparisons.";
    f.codeLine = 5;
    f.comparisons = comparisons;
    int position = 0;
    for (Node* temp = head; temp != nullptr; temp = temp->next) {
        f.cells.push_back(Cell(temp->data, ROLE_DONE));
        position++;
    }
    f.markers.push_back(Marker(0, "head"));
    Recorder::add(f);
}

void LinkedList::clear() {
    while (popFront()) {
    }
}

void LinkedList::display() const {
    if (head == nullptr) {
        cout << "List is empty." << endl;
        return;
    }
    Node* temp = head;
    while (temp != nullptr) {
        cout << temp->data << " -> ";
        temp = temp->next;
    }
    cout << "NULL" << endl;
}
