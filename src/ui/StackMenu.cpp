#include "StackMenu.h"
#include "ConsoleUtils.h"
#include "RecursionMenu.h"
#include "../core/ArrayStack.h"
#include "../core/LinkedStack.h"
#include "../core/StackApps.h"
#include <iostream>
using namespace std;

static void runArrayStackMenu() {
    ArrayStack stack;
    int choice = -1;

    while (choice != 0) {
        clearScreen();
        printTitle("Stack (array, size " + to_string(ArrayStack::MAX) + ")");
        stack.display();
        cout << "Size: " << stack.size() << " of " << ArrayStack::MAX << "\n" << endl;
        cout << " 1. Push" << endl;
        cout << " 2. Pop" << endl;
        cout << " 3. Peek" << endl;
        cout << " 4. Clear" << endl;
        cout << " 0. Back\n" << endl;

        choice = readInt("Your choice: ");
        int value;

        if (choice == 1) {
            value = readInt("Value to push: ");
            if (stack.push(value)) cout << value << " pushed." << endl;
            else cout << "Stack Overflow: the stack is full, " << value << " was not pushed." << endl;
            printComplexity("O(1)");
        }
        else if (choice == 2) {
            if (stack.pop(value)) cout << value << " popped." << endl;
            else cout << "Stack Underflow: the stack is empty." << endl;
            printComplexity("O(1)");
        }
        else if (choice == 3) {
            if (stack.peek(value)) cout << "Top element: " << value << endl;
            else cout << "The stack is empty." << endl;
            printComplexity("O(1)");
        }
        else if (choice == 4) {
            stack.clear();
            cout << "Stack cleared." << endl;
        }
        else if (choice != 0) {
            cout << "Invalid choice. Please pick a number from the menu." << endl;
        }

        if (choice != 0) {
            pauseScreen();
        }
    }
}

static void runLinkedStackMenu() {
    LinkedStack stack;
    int choice = -1;

    while (choice != 0) {
        clearScreen();
        printTitle("Stack (linked list)");
        stack.display();
        cout << "Size: " << stack.size() << "\n" << endl;
        cout << " 1. Push" << endl;
        cout << " 2. Pop" << endl;
        cout << " 3. Peek" << endl;
        cout << " 4. Clear" << endl;
        cout << " 0. Back\n" << endl;

        choice = readInt("Your choice: ");
        int value;

        if (choice == 1) {
            value = readInt("Value to push: ");
            stack.push(value);
            cout << value << " pushed." << endl;
            printComplexity("O(1)");
        }
        else if (choice == 2) {
            if (stack.pop(value)) cout << value << " popped." << endl;
            else cout << "Stack Underflow: the stack is empty." << endl;
            printComplexity("O(1)");
        }
        else if (choice == 3) {
            if (stack.peek(value)) cout << "Top element: " << value << endl;
            else cout << "The stack is empty." << endl;
            printComplexity("O(1)");
        }
        else if (choice == 4) {
            stack.clear();
            cout << "Stack cleared." << endl;
        }
        else if (choice != 0) {
            cout << "Invalid choice. Please pick a number from the menu." << endl;
        }

        if (choice != 0) {
            pauseScreen();
        }
    }
}

void runStackMenu() {
    int choice = -1;

    while (choice != 0) {
        clearScreen();
        printTitle("Stack, Applications and Recursion");
        cout << " 1. Stack using an array" << endl;
        cout << " 2. Stack using a linked list" << endl;
        cout << " 3. Check brackets in an expression" << endl;
        cout << " 4. Convert infix to postfix" << endl;
        cout << " 5. Evaluate a postfix expression" << endl;
        cout << " 6. Recursion lab" << endl;
        cout << " 0. Back to main menu\n" << endl;

        choice = readInt("Your choice: ");

        if (choice == 1) {
            runArrayStackMenu();
        }
        else if (choice == 2) {
            runLinkedStackMenu();
        }
        else if (choice == 3) {
            string text = readLine("Expression, for example {a+(b*c)}: ");
            if (isBalanced(text)) cout << "The brackets are balanced." << endl;
            else cout << "The brackets are NOT balanced." << endl;
            printComplexity("O(n), one stack push or pop per bracket");
            pauseScreen();
        }
        else if (choice == 4) {
            string text = readLine("Infix expression, for example (a+b)*c-d/e: ");
            bool ok;
            string postfix = infixToPostfix(text, ok);
            if (ok) cout << "Postfix: " << postfix << endl;
            else cout << "This is not a valid expression (check brackets and characters)." << endl;
            printComplexity("O(n)");
            pauseScreen();
        }
        else if (choice == 5) {
            string text = readLine("Postfix expression with spaces, for example 5 1 2 + 4 * + 3 - : ");
            int result;
            if (evaluatePostfix(text, result)) cout << "Result: " << result << endl;
            else cout << "This postfix expression cannot be evaluated (wrong operands, unknown token or division by zero)." << endl;
            printComplexity("O(n)");
            pauseScreen();
        }
        else if (choice == 6) {
            runRecursionMenu();
        }
        else if (choice != 0) {
            cout << "Invalid choice. Please pick a number from the menu." << endl;
            pauseScreen();
        }
    }
}
