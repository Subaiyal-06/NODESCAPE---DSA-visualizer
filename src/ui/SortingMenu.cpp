#include "SortingMenu.h"
#include "ConsoleUtils.h"
#include "ReplayPlayer.h"
#include "../core/Recorder.h"
#include "../core/Sorting.h"
#include "../core/BinaryHeap.h"
#include "../core/Trace.h"
#include <iostream>
#include <iomanip>
using namespace std;

static const int MAXN = 15;

static void runAlgorithm(int which, int a[], int n, SortStats& stats) {
    if (which == 1) bubbleSort(a, n, stats);
    else if (which == 2) selectionSort(a, n, stats);
    else if (which == 3) insertionSort(a, n, stats);
    else if (which == 4) mergeSort(a, n, stats);
    else if (which == 5) quickSort(a, n, stats);
    else heapSort(a, n, stats);
}

static const char* algorithmName(int which) {
    const char* names[] = {"", "Bubble sort", "Selection sort", "Insertion sort", "Merge sort", "Quick sort", "Heap sort"};
    return names[which];
}

static const char* algorithmComplexity(int which) {
    const char* text[] = {"", "O(n^2) worst and average, O(n) best", "O(n^2) in every case",
                          "O(n^2) worst and average, O(n) best", "O(n log n) in every case",
                          "O(n log n) average, O(n^2) worst", "O(n log n) in every case"};
    return text[which];
}

void runSortingMenu() {
    int data[MAXN] = {7, 3, 9, 1, 6, 4, 8, 2};
    int n = 8;
    int choice = -1;

    while (choice != 0) {
        clearScreen();
        printTitle("Sorting");
        cout << "Array: ";
        printIntArray(data, n);
        cout << "Size: " << n << "\n" << endl;

        cout << " 1. Type my own array" << endl;
        cout << " 2. Random array" << endl;
        cout << " 3. Already sorted array (best case)" << endl;
        cout << " 4. Reversed array (worst case)" << endl;
        cout << " 5. Run one sorting algorithm" << endl;
        cout << " 6. Compare all algorithms on this array" << endl;
        cout << " 0. Back to main menu\n" << endl;

        choice = readInt("Your choice: ");

        if (choice == 1) {
            n = readIntInRange("How many values (2 to " + to_string(MAXN) + "): ", 2, MAXN);
            for (int i = 0; i < n; i++) {
                data[i] = readInt("Value " + to_string(i + 1) + ": ");
            }
        }
        else if (choice == 2) {
            n = readIntInRange("How many values (2 to " + to_string(MAXN) + "): ", 2, MAXN);
            for (int i = 0; i < n; i++) {
                data[i] = randomInt(1, 99);
            }
        }
        else if (choice == 3 || choice == 4) {
            n = 12;
            for (int i = 0; i < n; i++) {
                data[i] = (choice == 3) ? i + 1 : n - i;
            }
        }
        else if (choice == 5) {
            cout << " 1. Bubble sort\n 2. Selection sort\n 3. Insertion sort\n 4. Merge sort\n 5. Quick sort\n 6. Heap sort" << endl;
            int which = readIntInRange("Algorithm: ", 1, 6);
            int copy[MAXN];
            for (int i = 0; i < n; i++) copy[i] = data[i];

            cout << "\n" << algorithmName(which) << endl;
            cout << "Before: ";
            printIntArray(copy, n);
            SortStats stats;
            runAlgorithm(which, copy, n, stats);
            cout << "After : ";
            printIntArray(copy, n);
            cout << "Comparisons: " << stats.comparisons << "   Swaps/moves: " << stats.swaps << endl;
            printComplexity(algorithmComplexity(which));
        }
        else if (choice == 6) {
            cout << left << setw(16) << "Algorithm" << setw(14) << "Comparisons" << setw(14) << "Swaps/moves" << "Time complexity" << endl;
            cout << "--------------------------------------------------------------------" << endl;
            bool stepModeWas = Trace::enabled;
            bool replayWas = Recorder::enabled();
            Trace::enabled = false; // the table would be unreadable with every step printed
            Recorder::setEnabled(false);
            for (int which = 1; which <= 6; which++) {
                int copy[MAXN];
                for (int i = 0; i < n; i++) copy[i] = data[i];
                SortStats stats;
                runAlgorithm(which, copy, n, stats);
                cout << left << setw(16) << algorithmName(which) << setw(14) << stats.comparisons
                     << setw(14) << stats.swaps << algorithmComplexity(which) << endl;
            }
            Trace::enabled = stepModeWas;
            Recorder::setEnabled(replayWas);
            cout << "\nTry options 3 and 4 first to see the best and the worst case." << endl;
        }
        else if (choice != 0) {
            cout << "Invalid choice. Please pick a number from the menu." << endl;
        }

        if (choice != 0) {
            finishOperation();
        }
    }
}
