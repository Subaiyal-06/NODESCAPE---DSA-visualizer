// Console tests for the Circular Linked List core.
// getNextOfLast() follows the link of the last node, it must give the first value.

#include <iostream>
#include "../src/core/CircularLinkedList.h"
using namespace std;

int passed = 0;
int failed = 0;

void check(bool condition, const char* name) {
    if (condition) {
        passed++;
    } else {
        failed++;
        cout << "FAIL: " << name << endl;
    }
}

// the last node must link back to the first node
bool isCircular(const CircularLinkedList& list) {
    int first, nextOfLast;
    return list.getFirst(first) && list.getNextOfLast(nextOfLast) && first == nextOfLast;
}

int main() {
    // empty list
    {
        CircularLinkedList list;
        int v;
        check(list.isEmpty() && list.size() == 0, "new list is empty");
        check(!list.deleteFirst(), "deleteFirst on empty list");
        check(!list.deleteLast(), "deleteLast on empty list");
        check(!list.deleteByValue(1), "deleteByValue on empty list");
        check(!list.deleteAtPosition(1), "deleteAtPosition on empty list");
        check(list.search(1) == -1, "search on empty list");
        check(!list.getFirst(v) && !list.getLast(v) && !list.getNextOfLast(v), "getters on empty list");
    }

    // insertion
    {
        CircularLinkedList list;
        int v;
        list.insertAtEnd(20);
        check(isCircular(list), "single node points to itself");
        list.insertAtBeginning(10);
        list.insertAtEnd(30);                                    // 10 20 30
        check(list.size() == 3, "size after three inserts");
        check(list.search(10) == 1 && list.search(20) == 2 && list.search(30) == 3, "order");
        check(list.getFirst(v) && v == 10, "first value");
        check(list.getLast(v) && v == 30, "last value");
        check(isCircular(list), "circular after inserts");
        check(list.insertAtPosition(2, 15), "insert at position 2");      // 10 15 20 30
        check(list.insertAtPosition(5, 40), "insert at end position");    // 10 15 20 30 40
        check(list.insertAtPosition(1, 5), "insert at position 1");       // 5 10 15 20 30 40
        check(!list.insertAtPosition(0, 1), "position 0 rejected");
        check(!list.insertAtPosition(8, 1), "position beyond size + 1 rejected");
        check(list.size() == 6 && list.search(5) == 1 && list.search(15) == 3 && list.search(40) == 6, "order after positional inserts");
        check(list.getLast(v) && v == 40 && isCircular(list), "tail is correct after positional inserts");
    }

    // deletion
    {
        CircularLinkedList list;
        int v;
        for (int i = 1; i <= 6; i++) list.insertAtEnd(i * 10);   // 10 20 30 40 50 60
        check(list.deleteFirst(), "deleteFirst");                // 20 30 40 50 60
        check(list.deleteLast(), "deleteLast");                  // 20 30 40 50
        check(list.getFirst(v) && v == 20 && list.getLast(v) && v == 50 && isCircular(list), "ends and link after deletes");
        check(list.deleteByValue(30), "deleteByValue middle");   // 20 40 50
        check(!list.deleteByValue(99), "deleteByValue missing");
        check(list.deleteAtPosition(2), "deleteAtPosition middle");       // 20 50
        check(!list.deleteAtPosition(3), "deleteAtPosition out of range");
        check(list.size() == 2 && isCircular(list), "two nodes left");
        check(list.deleteByValue(50), "deleteByValue tail");     // 20
        check(list.getLast(v) && v == 20 && isCircular(list), "one node left");
        check(list.deleteByValue(20), "deleteByValue only node");
        check(list.isEmpty() && list.size() == 0, "empty again");
        list.insertAtEnd(7);                                     // still works after being emptied
        list.insertAtEnd(8);
        check(list.search(8) == 2 && isCircular(list), "list works after being emptied");
        list.clear();
        check(list.isEmpty(), "clear");
    }

    cout << passed << " checks passed, " << failed << " failed." << endl;
    return failed == 0 ? 0 : 1;
}
