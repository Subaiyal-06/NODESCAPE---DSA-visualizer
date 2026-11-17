#include "DoublyCircularMenu.h"
#include "ConsoleUtils.h"
#include "DoublyListMenu.h"
#include "CircularListMenu.h"
#include <iostream>
using namespace std;

void runDoublyCircularMenu() {
    int choice = -1;

    while (choice != 0) {
        clearScreen();
        printTitle("Doubly and Circular Linked List");
        cout << " 1. Doubly Linked List" << endl;
        cout << " 2. Circular Linked List" << endl;
        cout << " 0. Back to main menu\n" << endl;

        choice = readInt("Your choice: ");

        if (choice == 1) {
            runDoublyListMenu();
        }
        else if (choice == 2) {
            runCircularListMenu();
        }
        else if (choice != 0) {
            cout << "Invalid choice. Please pick a number from the menu." << endl;
            pauseScreen();
        }
    }
}
