// Classic stack applications from the lectures

#ifndef STACKAPPS_H
#define STACKAPPS_H

#include <string>

// true when every (, [ and { has a matching closing bracket in the right order
bool isBalanced(const std::string& expression);

// Converts an infix expression to postfix. Operands are numbers or names, operators are + - * / % ^
// The result has a space after every token, for example "3 4 2 * +". ok is false for an invalid expression.
std::string infixToPostfix(const std::string& infix, bool& ok);

// Evaluates a postfix expression whose tokens are separated by spaces. Returns false when it is invalid.
bool evaluatePostfix(const std::string& postfix, int& result);

// recorded versions: the same steps, but every step is stored as a frame for the graphical app
bool checkBracketsRecorded(const std::string& expression);
std::string infixToPostfixRecorded(const std::string& infix, bool& ok);
bool evaluatePostfixRecorded(const std::string& postfix, int& result);

#endif
