#include "ArrayMenu.h"
#include "ConsoleUtils.h"
#include "../core/DynamicArray.h"
#include <iostream>
using namespace std;

void runArrayMenu() {
    DynamicArray arr(4);
    int choice = -1;

    while (choice != 0) {
        clearScreen();
        printTitle("Array and Searching");
        cout << "Array: ";
        arr.display();
        cout << "Size: " << arr.size() << "   Capacity: " << arr.getCapacity()
             << "   Sorted: " << (arr.isSorted() ? "yes" : "no") << "\n" << endl;

        cout << " 1. Append a value" << endl;
        cout << " 2. Insert at index" << endl;
        cout << " 3. Remove at index" << endl;
        cout << " 4. Linear search" << endl;
        cout << " 5. Binary search (needs a sorted array)" << endl;
        cout << " 6. Sort the array (ascending)" << endl;
        cout << " 7. Fill with random values" << endl;
        cout << " 8. Clear the array" << endl;
        cout << " 0. Back to main menu\n" << endl;

        choice = readInt("Your choice: ");

        if (choice == 1) {
            int value = readInt("Value to append: ");
            arr.append(value);
            cout << value << " appended." << endl;
            printComplexity("O(1) on average, O(n) when the array has to grow");
        }
        else if (choice == 2) {
            int value = readInt("Value to insert: ");
            int index = readInt("Index (0 to " + to_string(arr.size()) + "): ");
            if (arr.insertAt(index, value)) {
                cout << value << " inserted at index " << index << "." << endl;
                printComplexity("O(n), elements after the index shift to the right");
            } else {
                cout << "Invalid index. Nothing was inserted." << endl;
            }
        }
        else if (choice == 3) {
            int index = readInt("Index to remove: ");
            if (arr.removeAt(index)) {
                cout << "Element at index " << index << " removed." << endl;
                printComplexity("O(n), elements after the index shift to the left");
            } else {
                cout << "Invalid index. Nothing was removed." << endl;
            }
        }
        else if (choice == 4) {
            int key = readInt("Value to search: ");
            int index = arr.linearSearch(key);
            if (index == -1) cout << key << " was not found." << endl;
            else cout << key << " found at index " << index << "." << endl;
            cout << "Comparisons: " << arr.lastComparisons() << endl;
            printComplexity("O(n)");
        }
        else if (choice == 5) {
            int key = readInt("Value to search: ");
            int index = arr.binarySearch(key);
            if (index == -2) {
                cout << "The array is not sorted. Binary search needs sorted data, use option 6 first." << endl;
            } else {
                if (index == -1) cout << key << " was not found." << endl;
                else cout << key << " found at index " << index << "." << endl;
                cout << "Comparisons: " << arr.lastComparisons() << endl;
                printComplexity("O(log n), the search range is halved every time");
            }
        }
        else if (choice == 6) {
            arr.sortAscending();
            cout << "Array sorted." << endl;
            printComplexity("O(n^2), insertion sort");
        }
        else if (choice == 7) {
            int n = readIntInRange("How many values (1 to 20): ", 1, 20);
            arr.clear();
            for (int i = 0; i < n; i++) {
                arr.append(randomInt(1, 99));
            }
            cout << n << " random values added." << endl;
        }
        else if (choice == 8) {
            arr.clear();
            cout << "Array cleared." << endl;
        }
        else if (choice != 0) {
            cout << "Invalid choice. Please pick a number from the menu." << endl;
        }

        if (choice != 0) {
            pauseScreen();
        }
    }
}
