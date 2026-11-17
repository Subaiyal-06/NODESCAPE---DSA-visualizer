#include "QueueMenu.h"
#include "ConsoleUtils.h"
#include "../core/ArrayQueue.h"
#include "../core/CircularQueue.h"
#include "../core/Deque.h"
#include "../core/LinkedQueue.h"
#include <iostream>
using namespace std;

static void runArrayQueueMenu() {
    ArrayQueue queue;
    int choice = -1;

    while (choice != 0) {
        clearScreen();
        printTitle("Simple Queue (array, size " + to_string(ArrayQueue::MAX) + ")");
        queue.display();
        cout << "Size: " << queue.size() << "\n" << endl;
        cout << " 1. Enqueue" << endl;
        cout << " 2. Dequeue" << endl;
        cout << " 3. Peek front" << endl;
        cout << " 4. Clear" << endl;
        cout << " 0. Back\n" << endl;
        cout << "Note: after dequeues the freed space at the front is NOT reused. See the circular queue." << endl;

        choice = readInt("Your choice: ");
        int value;

        if (choice == 1) {
            value = readInt("Value to enqueue: ");
            if (queue.enqueue(value)) cout << value << " enqueued." << endl;
            else cout << "Queue Overflow: rear is at the end of the array." << endl;
            printComplexity("O(1)");
        }
        else if (choice == 2) {
            if (queue.dequeue(value)) cout << value << " dequeued." << endl;
            else cout << "Queue Underflow: the queue is empty." << endl;
            printComplexity("O(1)");
        }
        else if (choice == 3) {
            if (queue.peekFront(value)) cout << "Front element: " << value << endl;
            else cout << "The queue is empty." << endl;
        }
        else if (choice == 4) {
            queue.clear();
            cout << "Queue cleared." << endl;
        }
        else if (choice != 0) {
            cout << "Invalid choice. Please pick a number from the menu." << endl;
        }

        if (choice != 0) {
            pauseScreen();
        }
    }
}

static void runCircularQueueMenu() {
    CircularQueue queue;
    int choice = -1;

    while (choice != 0) {
        clearScreen();
        printTitle("Circular Queue (size " + to_string(CircularQueue::MAX) + ")");
        queue.display();
        cout << endl;
        cout << " 1. Enqueue" << endl;
        cout << " 2. Dequeue" << endl;
        cout << " 3. Peek front" << endl;
        cout << " 4. Peek rear" << endl;
        cout << " 5. Clear" << endl;
        cout << " 0. Back\n" << endl;

        choice = readInt("Your choice: ");
        int value;

        if (choice == 1) {
            value = readInt("Value to enqueue: ");
            if (queue.enqueue(value)) cout << value << " enqueued." << endl;
            else cout << "Queue Overflow: all slots are used." << endl;
            printComplexity("O(1)");
        }
        else if (choice == 2) {
            if (queue.dequeue(value)) cout << value << " dequeued." << endl;
            else cout << "Queue Underflow: the queue is empty." << endl;
            printComplexity("O(1)");
        }
        else if (choice == 3) {
            if (queue.peekFront(value)) cout << "Front element: " << value << endl;
            else cout << "The queue is empty." << endl;
        }
        else if (choice == 4) {
            if (queue.peekRear(value)) cout << "Rear element: " << value << endl;
            else cout << "The queue is empty." << endl;
        }
        else if (choice == 5) {
            queue.clear();
            cout << "Queue cleared." << endl;
        }
        else if (choice != 0) {
            cout << "Invalid choice. Please pick a number from the menu." << endl;
        }

        if (choice != 0) {
            pauseScreen();
        }
    }
}

static void runDequeMenu() {
    Deque deque;
    int choice = -1;

    while (choice != 0) {
        clearScreen();
        printTitle("Double Ended Queue (size " + to_string(Deque::MAX) + ")");
        deque.display();
        cout << "Size: " << deque.size() << "\n" << endl;
        cout << " 1. Insert at front" << endl;
        cout << " 2. Insert at rear" << endl;
        cout << " 3. Delete from front" << endl;
        cout << " 4. Delete from rear" << endl;
        cout << " 5. Clear" << endl;
        cout << " 0. Back\n" << endl;

        choice = readInt("Your choice: ");
        int value;

        if (choice == 1 || choice == 2) {
            value = readInt("Value to insert: ");
            bool ok = (choice == 1) ? deque.insertFront(value) : deque.insertRear(value);
            if (ok) cout << value << " inserted." << endl;
            else cout << "Deque Overflow: all slots are used." << endl;
            printComplexity("O(1)");
        }
        else if (choice == 3 || choice == 4) {
            bool ok = (choice == 3) ? deque.deleteFront(value) : deque.deleteRear(value);
            if (ok) cout << value << " deleted." << endl;
            else cout << "Deque Underflow: the deque is empty." << endl;
            printComplexity("O(1)");
        }
        else if (choice == 5) {
            deque.clear();
            cout << "Deque cleared." << endl;
        }
        else if (choice != 0) {
            cout << "Invalid choice. Please pick a number from the menu." << endl;
        }

        if (choice != 0) {
            pauseScreen();
        }
    }
}

static void runLinkedQueueMenu() {
    LinkedQueue queue;
    int choice = -1;

    while (choice != 0) {
        clearScreen();
        printTitle("Queue (linked list)");
        queue.display();
        cout << "Size: " << queue.size() << "\n" << endl;
        cout << " 1. Enqueue" << endl;
        cout << " 2. Dequeue" << endl;
        cout << " 3. Peek front" << endl;
        cout << " 4. Clear" << endl;
        cout << " 0. Back\n" << endl;

        choice = readInt("Your choice: ");
        int value;

        if (choice == 1) {
            value = readInt("Value to enqueue: ");
            queue.enqueue(value);
            cout << value << " enqueued." << endl;
            printComplexity("O(1), the tail pointer takes us to the rear directly");
        }
        else if (choice == 2) {
            if (queue.dequeue(value)) cout << value << " dequeued." << endl;
            else cout << "Queue Underflow: the queue is empty." << endl;
            printComplexity("O(1)");
        }
        else if (choice == 3) {
            if (queue.peekFront(value)) cout << "Front element: " << value << endl;
            else cout << "The queue is empty." << endl;
        }
        else if (choice == 4) {
            queue.clear();
            cout << "Queue cleared." << endl;
        }
        else if (choice != 0) {
            cout << "Invalid choice. Please pick a number from the menu." << endl;
        }

        if (choice != 0) {
            pauseScreen();
        }
    }
}

void runQueueMenu() {
    int choice = -1;

    while (choice != 0) {
        clearScreen();
        printTitle("Queues");
        cout << " 1. Simple queue (array)" << endl;
        cout << " 2. Circular queue" << endl;
        cout << " 3. Double ended queue (deque)" << endl;
        cout << " 4. Queue using a linked list" << endl;
        cout << " 0. Back to main menu\n" << endl;
        cout << "Priority queue is in the Heap and Priority Queue module." << endl;

        choice = readInt("Your choice: ");

        if (choice == 1) runArrayQueueMenu();
        else if (choice == 2) runCircularQueueMenu();
        else if (choice == 3) runDequeMenu();
        else if (choice == 4) runLinkedQueueMenu();
        else if (choice != 0) {
            cout << "Invalid choice. Please pick a number from the menu." << endl;
            pauseScreen();
        }
    }
}
