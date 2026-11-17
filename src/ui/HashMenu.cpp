#include "HashMenu.h"
#include "ConsoleUtils.h"
#include "../core/HashTable.h"
#include <iostream>
using namespace std;

static HashTable* createTable() {
    int size = readIntInRange("Table size (2 to " + to_string(HashTable::MAXSIZE) + ", a prime like 7, 11 or 13 works best): ", 2, HashTable::MAXSIZE);
    cout << " 1. Chaining\n 2. Linear probing\n 3. Quadratic probing\n 4. Double hashing" << endl;
    int method = readIntInRange("Collision method: ", 1, 4);
    cout << " 1. Division (key % size)\n 2. Folding\n 3. Mid-square" << endl;
    int function = readIntInRange("Hash function: ", 1, 3);

    CollisionMethod m = CHAINING;
    if (method == 2) m = LINEAR_PROBING;
    else if (method == 3) m = QUADRATIC_PROBING;
    else if (method == 4) m = DOUBLE_HASHING;

    HashFunction f = DIVISION;
    if (function == 2) f = FOLDING;
    else if (function == 3) f = MID_SQUARE;

    return new HashTable(size, m, f);
}

static const char* methodName(CollisionMethod m) {
    if (m == CHAINING) return "Chaining";
    if (m == LINEAR_PROBING) return "Linear probing";
    if (m == QUADRATIC_PROBING) return "Quadratic probing";
    return "Double hashing";
}

static const char* functionName(HashFunction f) {
    if (f == DIVISION) return "Division";
    if (f == FOLDING) return "Folding";
    return "Mid-square";
}

void runHashMenu() {
    HashTable* table = createTable();
    int choice = -1;

    while (choice != 0) {
        clearScreen();
        printTitle("Hashing");
        cout << "Method: " << methodName(table->getMethod()) << "   Hash function: " << functionName(table->getFunction())
             << "   Table size: " << table->getTableSize() << endl;
        table->display();
        cout << "Keys stored: " << table->itemCount() << "   Load factor: " << table->loadFactor() << "\n" << endl;
        cout << " 1. Insert a key" << endl;
        cout << " 2. Search a key" << endl;
        cout << " 3. Delete a key" << endl;
        cout << " 4. Insert 5 random keys" << endl;
        cout << " 5. Show the hash value of a key" << endl;
        cout << " 6. Clear the table" << endl;
        cout << " 7. Change the settings (this clears the table)" << endl;
        cout << " 0. Back to main menu\n" << endl;

        choice = readInt("Your choice: ");

        if (choice == 1) {
            int key = readInt("Key to insert (0 or more): ");
            if (table->insert(key)) {
                cout << key << " stored. Home slot " << table->hashValue(key) << ", collisions: " << table->lastCollisions()
                     << ", slots checked: " << table->lastProbes() << endl;
            } else if (key < 0) {
                cout << "Keys must be 0 or more." << endl;
            } else if (table->search(key) != -1) {
                cout << key << " is already in the table." << endl;
            } else {
                cout << "No free slot could be found for " << key << ". The table is full or the probing cannot reach a free slot." << endl;
            }
            printComplexity("O(1) on average, O(n) in the worst case");
        }
        else if (choice == 2) {
            int key = readInt("Key to search: ");
            int slot = table->search(key);
            if (slot == -1) cout << key << " was not found (" << table->lastProbes() << " slot(s) checked)." << endl;
            else cout << key << " found in slot " << slot << " (" << table->lastProbes() << " slot(s) checked)." << endl;
            printComplexity("O(1) on average, O(n) in the worst case");
        }
        else if (choice == 3) {
            int key = readInt("Key to delete: ");
            if (table->remove(key)) cout << key << " deleted." << endl;
            else cout << key << " was not found." << endl;
            printComplexity("O(1) on average, O(n) in the worst case");
        }
        else if (choice == 4) {
            int added = 0;
            int tries = 0;
            while (added < 5 && tries < 100) {
                if (table->insert(randomInt(1, 99))) added++;
                tries++;
            }
            cout << added << " random key(s) inserted." << endl;
        }
        else if (choice == 5) {
            int key = readInt("Key: ");
            if (key < 0) cout << "Keys must be 0 or more." << endl;
            else cout << "hash(" << key << ") = " << table->hashValue(key) << endl;
        }
        else if (choice == 6) {
            table->clear();
            cout << "Table cleared." << endl;
        }
        else if (choice == 7) {
            delete table;
            table = createTable();
        }
        else if (choice != 0) {
            cout << "Invalid choice. Please pick a number from the menu." << endl;
        }

        if (choice != 0) {
            pauseScreen();
        }
    }
    delete table;
}
