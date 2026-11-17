// Console tests for the Doubly Linked List core.
// search() walks forward from the head, searchFromEnd() walks backward from the tail.
// For every value: searchFromEnd = size - search + 1. This proves the prev links are correct.

#include <iostream>
#include "../src/core/DoublyLinkedList.h"
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

// true when forward and backward walks agree for the given values
bool linksAgree(const DoublyLinkedList& list, const int values[], int n) {
    for (int i = 0; i < n; i++) {
        int forward = list.search(values[i]);
        int backward = list.searchFromEnd(values[i]);
        if (forward == -1 || backward == -1 || backward != list.size() - forward + 1) {
            return false;
        }
    }
    return true;
}

int main() {
    // empty list
    {
        DoublyLinkedList list;
        check(list.isEmpty() && list.size() == 0, "new list is empty");
        check(!list.deleteFirst(), "deleteFirst on empty list");
        check(!list.deleteLast(), "deleteLast on empty list");
        check(!list.deleteByValue(1), "deleteByValue on empty list");
        check(!list.deleteAtPosition(1), "deleteAtPosition on empty list");
        check(list.search(1) == -1 && list.searchFromEnd(1) == -1, "search on empty list");
        list.reverse(); // must not crash
        list.clear();
        check(list.isEmpty(), "reverse and clear on empty list");
    }

    // insertion
    {
        DoublyLinkedList list;
        list.insertAtEnd(20);
        list.insertAtBeginning(10);
        list.insertAtEnd(30);                                   // 10 20 30
        check(list.size() == 3, "size after three inserts");
        check(list.search(10) == 1 && list.search(20) == 2 && list.search(30) == 3, "forward order");
        check(list.searchFromEnd(30) == 1 && list.searchFromEnd(10) == 3, "backward order");
        check(list.insertAtPosition(2, 15), "insert at position 2");     // 10 15 20 30
        check(list.insertAtPosition(5, 40), "insert at end position");   // 10 15 20 30 40
        check(list.insertAtPosition(1, 5), "insert at position 1");      // 5 10 15 20 30 40
        check(!list.insertAtPosition(0, 1), "position 0 rejected");
        check(!list.insertAtPosition(8, 1), "position beyond size + 1 rejected");
        int values[] = {5, 10, 15, 20, 30, 40};
        check(list.size() == 6 && linksAgree(list, values, 6), "links agree after inserts");
    }

    // deletion
    {
        DoublyLinkedList list;
        for (int i = 1; i <= 6; i++) list.insertAtEnd(i * 10);   // 10 20 30 40 50 60
        check(list.deleteFirst(), "deleteFirst");                // 20 30 40 50 60
        check(list.deleteLast(), "deleteLast");                  // 20 30 40 50
        check(list.deleteByValue(30), "deleteByValue middle");   // 20 40 50
        check(!list.deleteByValue(99), "deleteByValue missing");
        check(list.deleteAtPosition(2), "deleteAtPosition middle"); // 20 50
        check(!list.deleteAtPosition(3), "deleteAtPosition out of range");
        int values[] = {20, 50};
        check(list.size() == 2 && linksAgree(list, values, 2), "links agree after deletes");
        check(list.deleteByValue(50), "deleteByValue tail");     // 20
        check(list.deleteByValue(20), "deleteByValue only node");
        check(list.isEmpty(), "empty again");
        list.insertAtBeginning(7);                               // still works after being emptied
        list.insertAtEnd(8);
        int again[] = {7, 8};
        check(linksAgree(list, again, 2), "list works after being emptied");
    }

    // reverse
    {
        DoublyLinkedList list;
        for (int i = 1; i <= 4; i++) list.insertAtEnd(i);        // 1 2 3 4
        list.reverse();                                          // 4 3 2 1
        check(list.search(4) == 1 && list.search(1) == 4, "reverse order");
        int values[] = {1, 2, 3, 4};
        check(linksAgree(list, values, 4), "links agree after reverse");
        list.insertAtEnd(0);                                     // tail must be correct after reverse
        list.insertAtBeginning(9);
        check(list.search(9) == 1 && list.search(0) == 6 && list.searchFromEnd(0) == 1, "head and tail correct after reverse");
        list.clear();
        check(list.isEmpty(), "clear");
    }

    cout << passed << " checks passed, " << failed << " failed." << endl;
    return failed == 0 ? 0 : 1;
}
