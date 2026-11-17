#include "ConsoleUtils.h"
#include <iostream>
#include <limits>
#include <cstdlib>
#include <string>
using namespace std;

int readInt(const string& prompt) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // drop the rest of the line
            return value;
        }
        if (cin.eof()) {
            exit(0); // input is closed, nothing more to read
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Please enter a whole number." << endl;
    }
}

void pauseScreen() {
    cout << "\nPress Enter to continue...";
    cin.get();
}

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void printTitle(const string& title) {
    cout << "==============================" << endl;
    cout << "  " << title << endl;
    cout << "==============================" << endl;
}

void printComplexity(const string& timeComplexity) {
    cout << "Time complexity: " << timeComplexity << endl;
}

int readIntInRange(const string& prompt, int low, int high) {
    while (true) {
        int value = readInt(prompt);
        if (value >= low && value <= high) {
            return value;
        }
        cout << "Please enter a number from " << low << " to " << high << "." << endl;
    }
}

string readLine(const string& prompt) {
    cout << prompt;
    string line;
    if (!getline(cin, line)) {
        exit(0);
    }
    return line;
}

void printIntArray(const int a[], int n) {
    cout << "[ ";
    for (int i = 0; i < n; i++) {
        cout << a[i] << " ";
    }
    cout << "]" << endl;
}

int randomInt(int low, int high) {
    return low + rand() % (high - low + 1);
}
