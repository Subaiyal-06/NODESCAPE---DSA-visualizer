// NodeScape - Data Structures and Algorithms (CT-159)
// Phase 1: console version. Each data structure is one module.

#include <iostream>
#include <cstdlib>
#include <ctime>
#include "core/Trace.h"
#include "core/Recorder.h"
#include "ui/ConsoleUtils.h"
#include "ui/LinkedListMenu.h"
#include "ui/DoublyCircularMenu.h"
#include "ui/StackMenu.h"
#include "ui/QueueMenu.h"
#include "ui/SortingMenu.h"
#include "ui/TreeMenu.h"
#include "ui/AVLMenu.h"
#include "ui/HeapMenu.h"
#include "ui/GraphMenu.h"
#include "ui/HashMenu.h"
#include "ui/ArrayMenu.h"
#include "ui/ComplexityMenu.h"
using namespace std;

int main() {
    srand((unsigned int)time(nullptr)); // different random numbers every run
    int choice = -1;

    while (choice != 0) {
        clearScreen();
        printTitle("NodeScape - Data Structures");
        cout << "Step mode: " << (Trace::enabled ? "ON" : "OFF")
             << "     Replay mode: " << (Recorder::enabled() ? "ON" : "OFF") << "\n" << endl;
        cout << " 1. Singly Linked List" << endl;
        cout << " 2. Doubly and Circular Linked List" << endl;
        cout << " 3. Stack, Applications and Recursion" << endl;
        cout << " 4. Queues" << endl;
        cout << " 5. Sorting" << endl;
        cout << " 6. Binary Tree and BST" << endl;
        cout << " 7. AVL Tree" << endl;
        cout << " 8. Heap and Priority Queue" << endl;
        cout << " 9. Graph" << endl;
        cout << "10. Hashing" << endl;
        cout << "11. Array and Searching" << endl;
        cout << "12. Time Complexity" << endl;
        cout << "13. Turn step mode " << (Trace::enabled ? "OFF" : "ON") << endl;
        cout << "14. Turn replay mode " << (Recorder::enabled() ? "OFF" : "ON") << "   (watch steps forward and backward: linked list, sorting, BST)" << endl;
        cout << "15. Launch the desktop app (NodeScape)" << endl;
        cout << " 0. Exit\n" << endl;

        choice = readInt("Your choice: ");

        if (choice == 1) runLinkedListMenu();
        else if (choice == 2) runDoublyCircularMenu();
        else if (choice == 3) runStackMenu();
        else if (choice == 4) runQueueMenu();
        else if (choice == 5) runSortingMenu();
        else if (choice == 6) runTreeMenu();
        else if (choice == 7) runAVLMenu();
        else if (choice == 8) runHeapMenu();
        else if (choice == 9) runGraphMenu();
        else if (choice == 10) runHashMenu();
        else if (choice == 11) runArrayMenu();
        else if (choice == 12) runComplexityMenu();
        else if (choice == 13) {
            Trace::enabled = !Trace::enabled;
            cout << "Step mode is now " << (Trace::enabled ? "ON. Operations will explain every step." : "OFF.") << endl;
            pauseScreen();
        }
        else if (choice == 14) {
            Recorder::setEnabled(!Recorder::enabled());
            cout << "Replay mode is now " << (Recorder::enabled() ? "ON. After an operation in the linked list, sorting or BST module you can watch it step by step." : "OFF.") << endl;
            pauseScreen();
        }
        else if (choice == 15) {
            cout << "Launching the NodeScape desktop app..." << endl;
#ifdef _WIN32
            system("start build\\NodeScapeApp.exe");
#else
            system("./build/NodeScapeApp &");
#endif
            pauseScreen();
        }
        else if (choice != 0) {
            cout << "Invalid choice. Please pick a number from the menu." << endl;
            pauseScreen();
        }
    }

    cout << "Goodbye." << endl;
    return 0;
}
