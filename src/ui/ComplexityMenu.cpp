#include "ComplexityMenu.h"
#include "ConsoleUtils.h"
#include "../core/Complexity.h"
#include "../core/DynamicArray.h"
#include <iostream>
#include <iomanip>
#include <string>
using namespace std;

static void showGrowthTable() {
    long long sizes[] = {1, 5, 10, 20, 50, 100};
    cout << left << setw(11) << "Growth rate";
    for (int i = 0; i < 6; i++) {
        cout << setw(17) << ("n = " + to_string(sizes[i]));
    }
    cout << endl << string(11 + 6 * 17, '-') << endl;

    for (int k = 0; k < KIND_COUNT; k++) {
        cout << left << setw(11) << complexityName((ComplexityKind)k);
        for (int i = 0; i < 6; i++) {
            long long steps = stepsFor((ComplexityKind)k, sizes[i]);
            if (steps == -1) cout << setw(17) << "huge";
            else cout << setw(17) << steps;
        }
        cout << endl;
    }
    cout << "\n\"huge\" means the number is too big to count. See how fast O(2^n) runs away." << endl;
}

static void compareSearches() {
    int n = readIntInRange("Array size (1 to 100000): ", 1, 100000);
    DynamicArray arr(n);
    for (int i = 1; i <= n; i++) {
        arr.append(i * 2); // sorted: 2, 4, 6, ...
    }
    int missing = n * 2 + 1; // not in the array, so the search has to do its full work

    arr.linearSearch(missing);
    long linearSteps = arr.lastComparisons();
    arr.binarySearch(missing);
    long binarySteps = arr.lastComparisons();

    cout << "Searching for a value that is not in the array (the worst case):" << endl;
    cout << "  Linear search : " << linearSteps << " comparisons   O(n)" << endl;
    cout << "  Binary search : " << binarySteps << " comparisons   O(log n)" << endl;
    if (binarySteps > 0) {
        cout << "Binary search is about " << (linearSteps / binarySteps) << " times faster here." << endl;
    }
}

static void countLoopSteps() {
    int n = readIntInRange("n (1 to 3000): ", 1, 3000);

    long single = 0;
    for (int i = 0; i < n; i++) {
        single++;
    }

    long nested = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            nested++;
        }
    }

    long halving = 0;
    for (int i = n; i >= 1; i = i / 2) {
        halving++;
    }

    cout << "for (i = 0; i < n; i++)                  runs " << single << " times    O(n)" << endl;
    cout << "two nested loops over n                  runs " << nested << " times    O(n^2)" << endl;
    cout << "for (i = n; i >= 1; i = i / 2)           runs " << halving << " times    O(log n)" << endl;
}

void runComplexityMenu() {
    int choice = -1;

    while (choice != 0) {
        clearScreen();
        printTitle("Time Complexity");
        cout << " 1. How fast does each growth rate grow? (table)" << endl;
        cout << " 2. Linear search vs binary search (real step counts)" << endl;
        cout << " 3. Count the steps of real loops" << endl;
        cout << " 0. Back to main menu\n" << endl;

        choice = readInt("Your choice: ");

        if (choice == 1) showGrowthTable();
        else if (choice == 2) compareSearches();
        else if (choice == 3) countLoopSteps();
        else if (choice != 0) cout << "Invalid choice. Please pick a number from the menu." << endl;

        if (choice != 0) {
            pauseScreen();
        }
    }
}
