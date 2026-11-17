// Recursion examples. The traced versions print every call so the call stack can be seen.

#ifndef RECURSION_H
#define RECURSION_H

long long factorial(int n);                   // -1 for negative n or when n > 20 (would overflow)
long long factorialTraced(int n, int depth);  // prints every call and return, depth is used for indentation
long long fibonacci(int n, long long& calls); // naive recursion, calls counts how many calls were made
long long power(int base, int exponent);      // exponent must be 0 or more, otherwise -1
int gcd(int a, int b);
int sumOfDigits(int n);
int hanoi(int disks, char from, char to, char spare, bool print); // returns the number of moves

// recorded versions for the graphical app (call stack, call tree, pegs)
long long factorialRecorded(int n);   // n from 0 to 12
long long fibonacciRecorded(int n);   // n from 0 to 6
int hanoiRecorded(int disks);         // 1 to 5 disks, returns the number of moves

#endif
