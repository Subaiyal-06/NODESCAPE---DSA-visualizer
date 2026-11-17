#include "TreeMenu.h"
#include "ConsoleUtils.h"
#include "ReplayPlayer.h"
#include "../core/BinaryTree.h"
#include "../core/BST.h"
#include <iostream>
using namespace std;

static const int MAXNODES = 20; // keeps the picture of the tree readable on the console

static void showTraversal(const char* name, const int values[], int n) {
    cout << name << ": ";
    if (n == 0) {
        cout << "(tree is empty)" << endl;
        return;
    }
    printIntArray(values, n);
}

static void runBinaryTreeMenu() {
    BinaryTree tree;
    int choice = -1;
    int out[MAXNODES + 1];

    while (choice != 0) {
        clearScreen();
        printTitle("Binary Tree");
        tree.displayTree();
        cout << "\nSize: " << tree.size() << "   Height: " << tree.height() << "   Leaves: " << tree.leafCount() << "\n" << endl;
        cout << " 1. Insert a value (next free spot, level by level)" << endl;
        cout << " 2. Search a value" << endl;
        cout << " 3. Preorder traversal" << endl;
        cout << " 4. Inorder traversal" << endl;
        cout << " 5. Postorder traversal" << endl;
        cout << " 6. Level order traversal" << endl;
        cout << " 7. Fill with 7 random values" << endl;
        cout << " 8. Clear the tree" << endl;
        cout << " 0. Back\n" << endl;

        choice = readInt("Your choice: ");

        if (choice == 1) {
            if (tree.size() >= MAXNODES) {
                cout << "This console version shows up to " << MAXNODES << " nodes. Clear the tree to start again." << endl;
            } else {
                int value = readInt("Value to insert: ");
                tree.insertLevelOrder(value);
                cout << value << " inserted." << endl;
                printComplexity("O(n), we look for the first free spot level by level");
            }
        }
        else if (choice == 2) {
            int key = readInt("Value to search: ");
            if (tree.search(key)) cout << key << " is in the tree." << endl;
            else cout << key << " is not in the tree." << endl;
            printComplexity("O(n), the tree is not ordered, so every node may be checked");
        }
        else if (choice == 3) {
            showTraversal("Preorder (root, left, right)", out, tree.preorder(out, MAXNODES));
            printComplexity("O(n)");
        }
        else if (choice == 4) {
            showTraversal("Inorder (left, root, right)", out, tree.inorder(out, MAXNODES));
            printComplexity("O(n)");
        }
        else if (choice == 5) {
            showTraversal("Postorder (left, right, root)", out, tree.postorder(out, MAXNODES));
            printComplexity("O(n)");
        }
        else if (choice == 6) {
            showTraversal("Level order", out, tree.levelOrder(out, MAXNODES));
            printComplexity("O(n)");
        }
        else if (choice == 7) {
            tree.clear();
            for (int i = 0; i < 7; i++) {
                tree.insertLevelOrder(randomInt(1, 99));
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
            finishOperation();
        }
    }
}

static void runBSTMenu() {
    BST tree;
    int choice = -1;
    int out[MAXNODES + 1];

    while (choice != 0) {
        clearScreen();
        printTitle("Binary Search Tree");
        tree.displayTree();
        cout << "\nSize: " << tree.size() << "   Height: " << tree.height() << "\n" << endl;
        cout << " 1. Insert" << endl;
        cout << " 2. Delete" << endl;
        cout << " 3. Search" << endl;
        cout << " 4. Smallest and largest value" << endl;
        cout << " 5. Preorder traversal" << endl;
        cout << " 6. Inorder traversal (gives sorted order)" << endl;
        cout << " 7. Postorder traversal" << endl;
        cout << " 8. Level order traversal" << endl;
        cout << " 9. Fill with 7 random values" << endl;
        cout << "10. Clear the tree" << endl;
        cout << " 0. Back\n" << endl;

        choice = readInt("Your choice: ");

        if (choice == 1) {
            if (tree.size() >= MAXNODES) {
                cout << "This console version shows up to " << MAXNODES << " nodes. Clear the tree to start again." << endl;
            } else {
                int value = readInt("Value to insert: ");
                if (tree.insert(value)) cout << value << " inserted after " << tree.lastComparisons() << " comparison(s)." << endl;
                else cout << value << " is already in the tree. Duplicates are not allowed." << endl;
                printComplexity("O(h), where h is the height of the tree");
            }
        }
        else if (choice == 2) {
            int key = readInt("Value to delete: ");
            if (tree.remove(key)) cout << key << " deleted." << endl;
            else cout << key << " was not found." << endl;
            printComplexity("O(h)");
        }
        else if (choice == 3) {
            int key = readInt("Value to search: ");
            if (tree.search(key)) cout << key << " found after " << tree.lastComparisons() << " comparison(s)." << endl;
            else cout << key << " was not found (" << tree.lastComparisons() << " comparison(s))." << endl;
            printComplexity("O(h), O(log n) for a balanced tree and O(n) for a tree that looks like a list");
        }
        else if (choice == 4) {
            int smallest, largest;
            if (tree.findMin(smallest) && tree.findMax(largest)) {
                cout << "Smallest: " << smallest << " (keep going left)" << endl;
                cout << "Largest : " << largest << " (keep going right)" << endl;
            } else {
                cout << "The tree is empty." << endl;
            }
            printComplexity("O(h)");
        }
        else if (choice == 5) {
            showTraversal("Preorder", out, tree.preorder(out, MAXNODES));
            printComplexity("O(n)");
        }
        else if (choice == 6) {
            showTraversal("Inorder", out, tree.inorder(out, MAXNODES));
            printComplexity("O(n)");
        }
        else if (choice == 7) {
            showTraversal("Postorder", out, tree.postorder(out, MAXNODES));
            printComplexity("O(n)");
        }
        else if (choice == 8) {
            showTraversal("Level order", out, tree.levelOrder(out, MAXNODES));
            printComplexity("O(n)");
        }
        else if (choice == 9) {
            tree.clear();
            int added = 0;
            while (added < 7) {
                if (tree.insert(randomInt(1, 99))) added++;
            }
            cout << "Tree filled." << endl;
        }
        else if (choice == 10) {
            tree.clear();
            cout << "Tree cleared." << endl;
        }
        else if (choice != 0) {
            cout << "Invalid choice. Please pick a number from the menu." << endl;
        }

        if (choice != 0) {
            finishOperation();
        }
    }
}

void runTreeMenu() {
    int choice = -1;

    while (choice != 0) {
        clearScreen();
        printTitle("Binary Tree and BST");
        cout << " 1. Binary tree" << endl;
        cout << " 2. Binary search tree (BST)" << endl;
        cout << " 0. Back to main menu\n" << endl;

        choice = readInt("Your choice: ");

        if (choice == 1) runBinaryTreeMenu();
        else if (choice == 2) runBSTMenu();
        else if (choice != 0) {
            cout << "Invalid choice. Please pick a number from the menu." << endl;
            pauseScreen();
        }
    }
}
