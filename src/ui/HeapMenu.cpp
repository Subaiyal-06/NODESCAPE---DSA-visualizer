#include "HeapMenu.h"
#include "ConsoleUtils.h"
#include "../core/BinaryHeap.h"
#include "../core/PriorityQueue.h"
#include <iostream>
using namespace std;

static void runHeapOnly(bool isMax) {
    BinaryHeap heap(isMax);
    int choice = -1;

    while (choice != 0) {
        clearScreen();
        printTitle(isMax ? "Max Heap" : "Min Heap");
        heap.display();
        cout << "Size: " << heap.size() << " of " << BinaryHeap::MAX << "\n" << endl;
        cout << " 1. Insert" << endl;
        cout << " 2. Remove the top value (" << (isMax ? "largest" : "smallest") << ")" << endl;
        cout << " 3. Peek at the top value" << endl;
        cout << " 4. Build a heap from my own values (heapify)" << endl;
        cout << " 5. Fill with random values" << endl;
        cout << " 6. Clear" << endl;
        cout << " 0. Back\n" << endl;

        choice = readInt("Your choice: ");
        int value;

        if (choice == 1) {
            value = readInt("Value to insert: ");
            if (heap.insert(value)) cout << value << " inserted." << endl;
            else cout << "The heap is full (" << BinaryHeap::MAX << " values)." << endl;
            printComplexity("O(log n), the value floats up");
        }
        else if (choice == 2) {
            if (heap.extractTop(value)) cout << value << " removed." << endl;
            else cout << "The heap is empty." << endl;
            printComplexity("O(log n), the last value sinks down from the top");
        }
        else if (choice == 3) {
            if (heap.peek(value)) cout << "Top value: " << value << endl;
            else cout << "The heap is empty." << endl;
            printComplexity("O(1)");
        }
        else if (choice == 4) {
            int n = readIntInRange("How many values (1 to 15): ", 1, 15);
            int data[15];
            for (int i = 0; i < n; i++) data[i] = readInt("Value " + to_string(i + 1) + ": ");
            heap.buildHeap(data, n);
            cout << "Heap built." << endl;
            printComplexity("O(n), building from the bottom up");
        }
        else if (choice == 5) {
            int n = readIntInRange("How many values (1 to 15): ", 1, 15);
            heap.clear();
            for (int i = 0; i < n; i++) heap.insert(randomInt(1, 99));
            cout << n << " random values inserted." << endl;
        }
        else if (choice == 6) {
            heap.clear();
            cout << "Heap cleared." << endl;
        }
        else if (choice != 0) {
            cout << "Invalid choice. Please pick a number from the menu." << endl;
        }

        if (choice != 0) {
            pauseScreen();
        }
    }
}

static void runPriorityQueueMenu(bool largestFirst) {
    PriorityQueue queue(largestFirst);
    int choice = -1;

    while (choice != 0) {
        clearScreen();
        printTitle(largestFirst ? "Priority Queue (largest value first)" : "Priority Queue (smallest value first)");
        queue.display();
        cout << "Size: " << queue.size() << "\n" << endl;
        cout << " 1. Enqueue" << endl;
        cout << " 2. Dequeue (highest priority leaves first)" << endl;
        cout << " 3. Peek" << endl;
        cout << " 4. Fill with random values" << endl;
        cout << " 5. Clear" << endl;
        cout << " 0. Back\n" << endl;

        choice = readInt("Your choice: ");
        int value;

        if (choice == 1) {
            value = readInt("Value to enqueue: ");
            if (queue.enqueue(value)) cout << value << " enqueued." << endl;
            else cout << "The priority queue is full." << endl;
            printComplexity("O(log n)");
        }
        else if (choice == 2) {
            if (queue.dequeue(value)) cout << value << " dequeued." << endl;
            else cout << "The priority queue is empty." << endl;
            printComplexity("O(log n)");
        }
        else if (choice == 3) {
            if (queue.peek(value)) cout << "Next to leave: " << value << endl;
            else cout << "The priority queue is empty." << endl;
            printComplexity("O(1)");
        }
        else if (choice == 4) {
            queue.clear();
            for (int i = 0; i < 8; i++) queue.enqueue(randomInt(1, 99));
            cout << "8 random values enqueued." << endl;
        }
        else if (choice == 5) {
            queue.clear();
            cout << "Priority queue cleared." << endl;
        }
        else if (choice != 0) {
            cout << "Invalid choice. Please pick a number from the menu." << endl;
        }

        if (choice != 0) {
            pauseScreen();
        }
    }
}

static void runHeapSortDemo() {
    int n = readIntInRange("How many values (2 to 15): ", 2, 15);
    int data[15];
    for (int i = 0; i < n; i++) data[i] = randomInt(1, 99);

    cout << "Before: ";
    printIntArray(data, n);
    SortStats stats;
    heapSort(data, n, stats);
    cout << "After : ";
    printIntArray(data, n);
    cout << "Comparisons: " << stats.comparisons << "   Swaps: " << stats.swaps << endl;
    printComplexity("O(n log n) in every case");
}

void runHeapMenu() {
    int choice = -1;

    while (choice != 0) {
        clearScreen();
        printTitle("Heap and Priority Queue");
        cout << " 1. Max heap" << endl;
        cout << " 2. Min heap" << endl;
        cout << " 3. Priority queue (largest value first)" << endl;
        cout << " 4. Priority queue (smallest value first)" << endl;
        cout << " 5. Heap sort demo (random values)" << endl;
        cout << " 0. Back to main menu\n" << endl;

        choice = readInt("Your choice: ");

        if (choice == 1) runHeapOnly(true);
        else if (choice == 2) runHeapOnly(false);
        else if (choice == 3) runPriorityQueueMenu(true);
        else if (choice == 4) runPriorityQueueMenu(false);
        else if (choice == 5) {
            runHeapSortDemo();
            pauseScreen();
        }
        else if (choice != 0) {
            cout << "Invalid choice. Please pick a number from the menu." << endl;
            pauseScreen();
        }
    }
}
