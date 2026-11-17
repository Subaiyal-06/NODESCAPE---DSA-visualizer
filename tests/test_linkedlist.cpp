// Console tests for the Singly Linked List core.
// Build and run with: make test   (or run_tests.bat on Windows)

#include <iostream>
#include "../src/core/LinkedList.h"
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

int main() {
    // empty list
    {
        LinkedList list;
        check(list.isEmpty(), "new list is empty");
        check(list.size() == 0, "new list size is 0");
        check(!list.deleteFirst(), "deleteFirst on empty list returns false");
        check(!list.deleteLast(), "deleteLast on empty list returns false");
        check(!list.deleteByValue(5), "deleteByValue on empty list returns false");
        check(list.search(5) == -1, "search on empty list returns -1");
    }

    // insertion
    {
        LinkedList list;
        list.insertAtEnd(20);
        list.insertAtBeginning(10);
        list.insertAtEnd(30);              // 10 20 30
        check(list.size() == 3, "size after 3 inserts");
        check(list.search(10) == 1 && list.search(20) == 2 && list.search(30) == 3, "order after inserts");
        check(list.insertAtPosition(2, 15), "insert at position 2");        // 10 15 20 30
        check(list.insertAtPosition(5, 40), "insert at end position");      // 10 15 20 30 40
        check(list.insertAtPosition(1, 5), "insert at position 1");         // 5 10 15 20 30 40
        check(!list.insertAtPosition(0, 1), "position 0 is rejected");
        check(!list.insertAtPosition(8, 1), "position beyond size + 1 is rejected");
        check(list.size() == 6, "size after positional inserts");
        check(list.search(5) == 1 && list.search(15) == 3 && list.search(40) == 6, "order after positional inserts");
    }

    // deletion
    {
        LinkedList list;
        for (int i = 1; i <= 5; i++) list.insertAtEnd(i * 10); // 10 20 30 40 50
        check(list.deleteFirst(), "deleteFirst");                      // 20 30 40 50
        check(list.deleteLast(), "deleteLast");                        // 20 30 40
        check(list.deleteByValue(30), "deleteByValue middle");         // 20 40
        check(!list.deleteByValue(99), "deleteByValue missing value");
        check(list.deleteAtPosition(2), "deleteAtPosition last");      // 20
        check(!list.deleteAtPosition(2), "deleteAtPosition out of range");
        check(list.size() == 1 && list.search(20) == 1, "one node left");
        check(list.deleteLast(), "deleteLast on one node");
        check(list.isEmpty() && list.size() == 0, "empty again");
        list.insertAtEnd(7);                                           // list still works after being emptied
        check(list.search(7) == 1, "insert after emptying");
    }

    // reverse and sort
    {
        LinkedList list;
        list.insertAtEnd(5);
        list.insertAtEnd(1);
        list.insertAtEnd(3);
        list.reverse();                                                // 3 1 5
        check(list.search(3) == 1 && list.search(1) == 2 && list.search(5) == 3, "reverse");
        list.sortAscending();                                          // 1 3 5
        check(list.search(1) == 1 && list.search(3) == 2 && list.search(5) == 3, "sort ascending");
        list.insertAtEnd(3);                                           // duplicates
        list.sortAscending();                                          // 1 3 3 5
        check(list.size() == 4 && list.search(5) == 4, "sort with duplicates");
        list.clear();
        check(list.isEmpty(), "clear");
    }

    cout << passed << " checks passed, " << failed << " failed." << endl;
    return failed == 0 ? 0 : 1;
}
