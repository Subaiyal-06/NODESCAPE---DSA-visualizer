#include "DoublyListMenu.h"
#include "ConsoleUtils.h"
#include "../core/DoublyLinkedList.h"
#include <iostream>
using namespace std;

void runDoublyListMenu() {
    DoublyLinkedList list;
    int choice = -1;

    while (choice != 0) {
        clearScreen();
        printTitle("Doubly Linked List");
        cout << "Forward : ";
        list.displayForward();
        cout << "Backward: ";
        list.displayBackward();
        cout << "Size: " << list.size() << "\n" << endl;

        cout << " 1. Insert at beginning" << endl;
        cout << " 2. Insert at end" << endl;
        cout << " 3. Insert at position" << endl;
        cout << " 4. Delete first node" << endl;
        cout << " 5. Delete last node" << endl;
        cout << " 6. Delete by value" << endl;
        cout << " 7. Delete at position" << endl;
        cout << " 8. Search from the head" << endl;
        cout << " 9. Search from the tail" << endl;
        cout << "10. Reverse the list" << endl;
        cout << "11. Clear the list" << endl;
        cout << " 0. Back\n" << endl;

        choice = readInt("Your choice: ");

        if (choice == 1) {
            int value = readInt("Value to insert: ");
            list.insertAtBeginning(value);
            cout << value << " inserted at the beginning." << endl;
            printComplexity("O(1)");
        }
        else if (choice == 2) {
            int value = readInt("Value to insert: ");
            list.insertAtEnd(value);
            cout << value << " inserted at the end." << endl;
            printComplexity("O(1), the tail pointer takes us there directly");
        }
        else if (choice == 3) {
            int value = readInt("Value to insert: ");
            int position = readInt("Position (1 to " + to_string(list.size() + 1) + "): ");
            if (list.insertAtPosition(position, value)) {
                cout << value << " inserted at position " << position << "." << endl;
                printComplexity("O(n)");
            } else {
                cout << "Invalid position. Nothing was inserted." << endl;
            }
        }
        else if (choice == 4) {
            if (list.deleteFirst()) {
                cout << "First node deleted." << endl;
                printComplexity("O(1)");
            } else {
                cout << "The list is empty. Nothing to delete." << endl;
            }
        }
        else if (choice == 5) {
            if (list.deleteLast()) {
                cout << "Last node deleted." << endl;
                printComplexity("O(1), the tail and prev pointers make it direct");
            } else {
                cout << "The list is empty. Nothing to delete." << endl;
            }
        }
        else if (choice == 6) {
            int value = readInt("Value to delete: ");
            if (list.deleteByValue(value)) {
                cout << value << " deleted." << endl;
                printComplexity("O(n)");
            } else {
                cout << value << " was not found." << endl;
            }
        }
        else if (choice == 7) {
            int position = readInt("Position to delete: ");
            if (list.deleteAtPosition(position)) {
                cout << "Node at position " << position << " deleted." << endl;
                printComplexity("O(n)");
            } else {
                cout << "Invalid position. Nothing was deleted." << endl;
            }
        }
        else if (choice == 8) {
            int value = readInt("Value to search: ");
            int position = list.search(value);
            if (position == -1) cout << value << " was not found." << endl;
            else cout << value << " found at position " << position << " from the head." << endl;
            printComplexity("O(n)");
        }
        else if (choice == 9) {
            int value = readInt("Value to search: ");
            int position = list.searchFromEnd(value);
            if (position == -1) cout << value << " was not found." << endl;
            else cout << value << " found at position " << position << " from the tail." << endl;
            printComplexity("O(n)");
        }
        else if (choice == 10) {
            list.reverse();
            cout << "List reversed." << endl;
            printComplexity("O(n)");
        }
        else if (choice == 11) {
            list.clear();
            cout << "List cleared." << endl;
            printComplexity("O(n)");
        }
        else if (choice != 0) {
            cout << "Invalid choice. Please pick a number from the menu." << endl;
        }

        if (choice != 0) {
            pauseScreen();
        }
    }
}
