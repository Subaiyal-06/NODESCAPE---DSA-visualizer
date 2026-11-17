// Small helpers for reading input safely and drawing simple console screens

#ifndef CONSOLEUTILS_H
#define CONSOLEUTILS_H

#include <string>

int readInt(const std::string& prompt); // keeps asking until the user types a whole number
void pauseScreen();                     // waits for Enter
void clearScreen();
void printTitle(const std::string& title);
void printComplexity(const std::string& timeComplexity); // prints a line like "Time complexity: O(n)"
int readIntInRange(const std::string& prompt, int low, int high); // asks again until the number is in range
std::string readLine(const std::string& prompt);                   // reads a whole line of text
void printIntArray(const int a[], int n);                          // prints [ 1 2 3 ]
int randomInt(int low, int high);                                  // random number from low to high

#endif
