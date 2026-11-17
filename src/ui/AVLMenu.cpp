#include "AVLMenu.h"
#include "ConsoleUtils.h"
#include "../core/AVLTree.h"
#include <iostream>
using namespace std;

static const int MAXNODES = 20;

void runAVLMenu() {
    AVLTree tree;
    int choice = -1;
    int out[MAXNODES + 1];

    while (choice != 0) {
        clearScreen();
        printTitle("AVL Tree");
        tree.displayTree();
        cout << "\nSize: " << tree.size() << "   Height: " << tree.height()
             << "   Rotations so far: " << tree.rotationCount() << "\n" << endl;
        cout << " 1. Insert" << endl;
        cout << " 2. Delete" << endl;
        cout << " 3. Search" << endl;
        cout << " 4. Inorder traversal" << endl;
        cout << " 5. Preorder traversal" << endl;
        cout << " 6. Demo: insert 10, 20, 30, 40, 50, 25 one by one" << endl;
        cout << " 7. Fill with 7 random values" << endl;
        cout << " 8. Clear the tree" << endl;
        cout << " 0. Back to main menu\n" << endl;
        cout << "Tip: switch step mode on in the main menu to see every rotation explained." << endl;

        choice = readInt("Your choice: ");

        if (choice == 1) {
            if (tree.size() >= MAXNODES) {
                cout << "This console version shows up to " << MAXNODES << " nodes. Clear the tree to start again." << endl;
            } else {
                int value = readInt("Value to insert: ");
                int before = tree.rotationCount();
                if (tree.insert(value)) {
                    cout << value << " inserted. Rotations needed: " << (tree.rotationCount() - before) << endl;
                } else {
                    cout << value << " is already in the tree. Duplicates are not allowed." << endl;
                }
                printComplexity("O(log n), the tree is always balanced");
            }
        }
        else if (choice == 2) {
            int key = readInt("Value to delete: ");
            int before = tree.rotationCount();
            if (tree.remove(key)) cout << key << " deleted. Rotations needed: " << (tree.rotationCount() - before) << endl;
            else cout << key << " was not found." << endl;
            printComplexity("O(log n)");
        }
        else if (choice == 3) {
            int key = readInt("Value to search: ");
            if (tree.search(key)) cout << key << " is in the tree." << endl;
            else cout << key << " is not in the tree." << endl;
            printComplexity("O(log n)");
        }
        else if (choice == 4 || choice == 5) {
            int n = (choice == 4) ? tree.inorder(out, MAXNODES) : tree.preorder(out, MAXNODES);
            cout << (choice == 4 ? "Inorder: " : "Preorder: ");
            if (n == 0) cout << "(tree is empty)" << endl;
            else printIntArray(out, n);
            printComplexity("O(n)");
        }
        else if (choice == 6) {
            tree.clear();
            int values[] = {10, 20, 30, 40, 50, 25};
            for (int i = 0; i < 6; i++) {
                int before = tree.rotationCount();
                tree.insert(values[i]);
                cout << "\nInserted " << values[i] << " (rotations: " << (tree.rotationCount() - before) << ")" << endl;
                tree.displayTree();
            }
        }
        else if (choice == 7) {
            tree.clear();
            int added = 0;
            while (added < 7) {
                if (tree.insert(randomInt(1, 99))) added++;
            }
            cout << "Tree filled." << endl;
        }
        else if (choice == 8) {
            tree.clear();
            cout << "Tree cleared." << endl;
        }
        else if (choice != 0) {
            cout << "Invalid choice. Please pick a number from the menu." << endl;
        }

        if (choice != 0) {
            pauseScreen();
        }
    }
}
