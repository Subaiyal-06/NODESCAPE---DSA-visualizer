#include "RecursionMenu.h"
#include "ConsoleUtils.h"
#include "../core/Recursion.h"
#include <iostream>
using namespace std;

void runRecursionMenu() {
    int choice = -1;

    while (choice != 0) {
        clearScreen();
        printTitle("Recursion Lab");
        cout << " 1. Factorial (with every call shown)" << endl;
        cout << " 2. Fibonacci (and how many calls it needs)" << endl;
        cout << " 3. Power" << endl;
        cout << " 4. GCD" << endl;
        cout << " 5. Sum of digits" << endl;
        cout << " 6. Tower of Hanoi" << endl;
        cout << " 0. Back\n" << endl;

        choice = readInt("Your choice: ");

        if (choice == 1) {
            int n = readIntInRange("n (0 to 20): ", 0, 20);
            long long result = factorialTraced(n, 0);
            cout << "\nfactorial(" << n << ") = " << result << endl;
            printComplexity("O(n), n calls are stacked on top of each other");
        }
        else if (choice == 2) {
            int n = readIntInRange("n (0 to 35): ", 0, 35);
            long long calls = 0;
            long long result = fibonacci(n, calls);
            cout << "fibonacci(" << n << ") = " << result << endl;
            cout << "Function calls needed: " << calls << endl;
            printComplexity("O(2^n), the same values are calculated again and again");
        }
        else if (choice == 3) {
            int base = readInt("Base: ");
            int exponent = readInt("Exponent (0 or more): ");
            long long result = power(base, exponent);
            if (result == -1 && exponent < 0) cout << "The exponent must be 0 or more." << endl;
            else cout << base << " ^ " << exponent << " = " << result << endl;
            printComplexity("O(n), where n is the exponent");
        }
        else if (choice == 4) {
            int a = readInt("First number: ");
            int b = readInt("Second number: ");
            cout << "gcd(" << a << ", " << b << ") = " << gcd(a, b) << endl;
            printComplexity("O(log n), Euclid's algorithm");
        }
        else if (choice == 5) {
            int n = readInt("Number: ");
            cout << "Sum of the digits of " << n << " = " << sumOfDigits(n) << endl;
            printComplexity("O(d), where d is the number of digits");
        }
        else if (choice == 6) {
            int disks = readIntInRange("Number of disks (1 to 6): ", 1, 6);
            int moves = hanoi(disks, 'A', 'C', 'B', true);
            cout << "Total moves: " << moves << endl;
            printComplexity("O(2^n), the moves double with every extra disk");
        }
        else if (choice != 0) {
            cout << "Invalid choice. Please pick a number from the menu." << endl;
        }

        if (choice != 0) {
            pauseScreen();
        }
    }
}
