#include "StackApps.h"
#include "LinkedStack.h"
#include "Trace.h"
using namespace std;

static bool isOpening(char c) {
    return c == '(' || c == '[' || c == '{';
}

static bool isClosing(char c) {
    return c == ')' || c == ']' || c == '}';
}

static bool matches(char open, char close) {
    return (open == '(' && close == ')') || (open == '[' && close == ']') || (open == '{' && close == '}');
}

static bool isOperator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || c == '^';
}

static int precedence(char op) {
    if (op == '^') return 3;
    if (op == '*' || op == '/' || op == '%') return 2;
    if (op == '+' || op == '-') return 1;
    return 0;
}

bool isBalanced(const string& expression) {
    LinkedStack stack;
    for (size_t i = 0; i < expression.size(); i++) {
        char c = expression[i];
        if (isOpening(c)) {
            stack.push(c);
            Trace::say(string("push ") + c);
        }
        else if (isClosing(c)) {
            int top;
            if (!stack.pop(top)) {
                Trace::say(string("found ") + c + " but the stack is empty");
                return false;
            }
            if (!matches((char)top, c)) {
                Trace::say(string("found ") + c + " but the top was " + (char)top);
                return false;
            }
            Trace::say(string("pop ") + (char)top + ", it matches " + c);
        }
    }
    return stack.isEmpty();
}

string infixToPostfix(const string& infix, bool& ok) {
    LinkedStack stack;
    string output = "";
    ok = true;
    size_t i = 0;

    while (i < infix.size()) {
        char c = infix[i];

        if (c == ' ') {
            i++;
        }
        else if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
            string token = "";
            while (i < infix.size() && ((infix[i] >= '0' && infix[i] <= '9') ||
                   (infix[i] >= 'a' && infix[i] <= 'z') || (infix[i] >= 'A' && infix[i] <= 'Z'))) {
                token += infix[i];
                i++;
            }
            output += token + " ";
            Trace::say("operand " + token + " goes to the output");
        }
        else if (c == '(') {
            stack.push(c);
            i++;
        }
        else if (c == ')') {
            int top;
            bool foundOpen = false;
            while (stack.pop(top)) {
                if (top == '(') {
                    foundOpen = true;
                    break;
                }
                output += string(1, (char)top) + " ";
            }
            if (!foundOpen) {
                ok = false;
                return "";
            }
            i++;
        }
        else if (isOperator(c)) {
            int top;
            while (stack.peek(top) && top != '(' &&
                   (precedence((char)top) > precedence(c) ||
                    (precedence((char)top) == precedence(c) && c != '^'))) {
                stack.pop(top);
                output += string(1, (char)top) + " ";
            }
            stack.push(c);
            i++;
        }
        else {
            ok = false; // a character we do not understand
            return "";
        }
    }

    int top;
    while (stack.pop(top)) {
        if (top == '(') {
            ok = false; // an opening bracket was never closed
            return "";
        }
        output += string(1, (char)top) + " ";
    }

    if (!output.empty()) {
        output.erase(output.size() - 1); // remove the last space
    }
    return output;
}

bool evaluatePostfix(const string& postfix, int& result) {
    LinkedStack stack;
    size_t i = 0;

    while (i < postfix.size()) {
        if (postfix[i] == ' ') {
            i++;
            continue;
        }
        string token = "";
        while (i < postfix.size() && postfix[i] != ' ') {
            token += postfix[i];
            i++;
        }

        bool isNumber = !token.empty();
        for (size_t k = 0; k < token.size(); k++) {
            bool minusAtStart = (k == 0 && token[k] == '-' && token.size() > 1);
            if (!(token[k] >= '0' && token[k] <= '9') && !minusAtStart) {
                isNumber = false;
            }
        }

        if (isNumber) {
            stack.push(stoi(token));
        }
        else if (token.size() == 1 && isOperator(token[0])) {
            int b, a;
            if (!stack.pop(b) || !stack.pop(a)) {
                return false; // not enough operands
            }
            int value = 0;
            char op = token[0];
            if (op == '+') value = a + b;
            else if (op == '-') value = a - b;
            else if (op == '*') value = a * b;
            else if (op == '/') {
                if (b == 0) return false;
                value = a / b;
            }
            else if (op == '%') {
                if (b == 0) return false;
                value = a % b;
            }
            else {
                if (b < 0) return false;
                value = 1;
                for (int p = 0; p < b; p++) {
                    value *= a;
                }
            }
            Trace::say(to_string(a) + " " + op + " " + to_string(b) + " = " + to_string(value));
            stack.push(value);
        }
        else {
            return false; // a name or a symbol cannot be evaluated
        }
    }

    if (stack.size() != 1) {
        return false;
    }
    stack.pop(result);
    return true;
}

// ---------- recorded versions (for the graphical app) ----------
// They follow the same steps as the functions above but keep their own small stack, so every
// step can be drawn: stack contents, the part of the expression that is being read, the output.

#include "Recorder.h"
#include <vector>

namespace {
    const vector<string> CODE_BRACKETS = {
        "for each character c of the expression:",
        "  if c is ( [ {  : push c",
        "  if c is ) ] }  : if the stack is empty -> not balanced",
        "                   pop the top; if it does not match c -> not balanced",
        "at the end: balanced only if the stack is empty"
    };
    const vector<string> CODE_POSTFIX = {
        "for each token of the infix expression:",
        "  operand  -> append to the output",
        "  (        -> push",
        "  )        -> pop to the output until ( is popped",
        "  operator -> pop operators with higher or equal precedence, then push it",
        "at the end: pop everything to the output"
    };
    const vector<string> CODE_EVAL = {
        "for each token of the postfix expression:",
        "  number   -> push it",
        "  operator -> b = pop();  a = pop();",
        "              push(a operator b);",
        "at the end: the only value left is the result"
    };

    void stackFrame(const vector<int>& st, bool chars, const string& message, int line, Role topRole, Role secondRole,
                    const vector<string>& info) {
        if (!Recorder::enabled()) return;
        Frame f;
        f.kind = KIND_STACK;
        f.message = message;
        f.codeLine = line;
        f.auxLabel = chars ? "chars" : "";
        for (size_t i = 0; i < st.size(); i++) {
            Role r = ROLE_NORMAL;
            if (i + 1 == st.size()) r = topRole;
            else if (i + 2 == st.size()) r = secondRole;
            f.cells.push_back(Cell(st[i], r));
        }
        if (!st.empty()) f.markers.push_back(Marker((int)st.size() - 1, "top"));
        f.info = info;
        Recorder::add(f);
    }

    string spaced(const string& s) {
        string out;
        for (size_t i = 0; i < s.size(); i++) {
            out += s[i];
            if (i + 1 < s.size()) out += ' ';
        }
        return out;
    }
}

bool checkBracketsRecorded(const string& expression) {
    Recorder::begin("Bracket matching", CODE_BRACKETS);
    vector<int> stack;
    string shown = spaced(expression);
    vector<string> info;
    info.push_back("Expression: " + expression);
    stackFrame(stack, true, "Start with an empty stack. Read the expression from left to right.", 0, ROLE_NORMAL, ROLE_NORMAL, info);
    for (size_t i = 0; i < expression.size(); i++) {
        char c = expression[i];
        info.clear();
        info.push_back("Expression: " + expression);
        info.push_back("Reading character " + to_string(i + 1) + ": " + string(1, c));
        if (isOpening(c)) {
            stack.push_back(c);
            stackFrame(stack, true, string(1, c) + " is an opening bracket: push it.", 1, ROLE_NEW, ROLE_NORMAL, info);
        }
        else if (isClosing(c)) {
            if (stack.empty()) {
                stackFrame(stack, true, string(1, c) + " is a closing bracket but the stack is empty: nothing to match. Not balanced.", 2, ROLE_KEPT, ROLE_NORMAL, info);
                return false;
            }
            int top = stack.back();
            stackFrame(stack, true, string(1, c) + " is a closing bracket. Compare it with the top of the stack, " + string(1, (char)top) + ".", 3, ROLE_CURRENT, ROLE_NORMAL, info);
            if (!matches((char)top, c)) {
                stackFrame(stack, true, string(1, (char)top) + " does not match " + string(1, c) + ". Not balanced.", 3, ROLE_KEPT, ROLE_NORMAL, info);
                return false;
            }
            stack.pop_back();
            stackFrame(stack, true, string(1, (char)top) + " matches " + string(1, c) + ": pop it.", 3, ROLE_FOUND, ROLE_NORMAL, info);
        }
    }
    info.clear();
    info.push_back("Expression: " + expression);
    bool ok = stack.empty();
    stackFrame(stack, true, ok ? "The whole expression was read and the stack is empty: balanced." : "The expression ended but the stack is not empty: some bracket was never closed. Not balanced.", 4, ok ? ROLE_DONE : ROLE_KEPT, ROLE_NORMAL, info);
    return ok;
}

string infixToPostfixRecorded(const string& infix, bool& ok) {
    Recorder::begin("Infix to postfix", CODE_POSTFIX);
    vector<int> stack;
    string output;
    ok = true;
    auto info = [&](const string& reading) {
        vector<string> lines;
        lines.push_back("Infix:   " + infix);
        lines.push_back("Reading: " + reading);
        lines.push_back("Output:  " + output);
        return lines;
    };
    stackFrame(stack, true, "Start with an empty stack and an empty output.", 0, ROLE_NORMAL, ROLE_NORMAL, info("-"));
    size_t i = 0;
    while (i < infix.size()) {
        char c = infix[i];
        if (c == ' ') {
            i++;
        }
        else if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
            string token = "";
            while (i < infix.size() && ((infix[i] >= '0' && infix[i] <= '9') ||
                   (infix[i] >= 'a' && infix[i] <= 'z') || (infix[i] >= 'A' && infix[i] <= 'Z'))) {
                token += infix[i];
                i++;
            }
            output += token + " ";
            stackFrame(stack, true, token + " is an operand: it goes straight to the output.", 1, ROLE_NORMAL, ROLE_NORMAL, info(token));
        }
        else if (c == '(') {
            stack.push_back(c);
            stackFrame(stack, true, "( is pushed on the stack.", 2, ROLE_NEW, ROLE_NORMAL, info("("));
            i++;
        }
        else if (c == ')') {
            bool foundOpen = false;
            while (!stack.empty()) {
                int top = stack.back();
                stack.pop_back();
                if (top == '(') {
                    foundOpen = true;
                    stackFrame(stack, true, "The matching ( is popped and thrown away.", 3, ROLE_NORMAL, ROLE_NORMAL, info(")"));
                    break;
                }
                output += string(1, (char)top) + " ";
                stackFrame(stack, true, string(1, (char)top) + " is popped to the output.", 3, ROLE_NORMAL, ROLE_NORMAL, info(")"));
            }
            if (!foundOpen) {
                ok = false;
                stackFrame(stack, true, "There is no matching ( for this ). The expression is invalid.", 3, ROLE_KEPT, ROLE_NORMAL, info(")"));
                return "";
            }
            i++;
        }
        else if (isOperator(c)) {
            string r(1, c);
            while (!stack.empty() && stack.back() != '(' &&
                   (precedence((char)stack.back()) > precedence(c) ||
                    (precedence((char)stack.back()) == precedence(c) && c != '^'))) {
                int top = stack.back();
                stackFrame(stack, true, string(1, (char)top) + " has higher or equal precedence than " + r + ": pop it to the output.", 4, ROLE_CURRENT, ROLE_NORMAL, info(r));
                stack.pop_back();
                output += string(1, (char)top) + " ";
            }
            stack.push_back(c);
            stackFrame(stack, true, r + " is pushed on the stack.", 4, ROLE_NEW, ROLE_NORMAL, info(r));
            i++;
        }
        else {
            ok = false; // a character we do not understand
            stackFrame(stack, true, "The character " + string(1, c) + " is not understood. The expression is invalid.", 0, ROLE_KEPT, ROLE_NORMAL, info(string(1, c)));
            return "";
        }
    }
    while (!stack.empty()) {
        int top = stack.back();
        stack.pop_back();
        if (top == '(') {
            ok = false; // an opening bracket was never closed
            stackFrame(stack, true, "A ( was never closed. The expression is invalid.", 5, ROLE_KEPT, ROLE_NORMAL, info("end"));
            return "";
        }
        output += string(1, (char)top) + " ";
        stackFrame(stack, true, "End of input: pop " + string(1, (char)top) + " to the output.", 5, ROLE_NORMAL, ROLE_NORMAL, info("end"));
    }
    if (!output.empty()) {
        output.erase(output.size() - 1); // remove the last space
    }
    stackFrame(stack, true, "Done. The postfix expression is: " + output, 5, ROLE_DONE, ROLE_NORMAL, info("end"));
    return output;
}

bool evaluatePostfixRecorded(const string& postfix, int& result) {
    Recorder::begin("Postfix evaluation", CODE_EVAL);
    vector<int> stack;
    auto info = [&](const string& reading, const string& step) {
        vector<string> lines;
        lines.push_back("Postfix: " + postfix);
        lines.push_back("Reading: " + reading);
        if (!step.empty()) lines.push_back(step);
        return lines;
    };
    stackFrame(stack, false, "Start with an empty stack.", 0, ROLE_NORMAL, ROLE_NORMAL, info("-", ""));
    size_t i = 0;
    while (i < postfix.size()) {
        if (postfix[i] == ' ') {
            i++;
            continue;
        }
        string token = "";
        while (i < postfix.size() && postfix[i] != ' ') {
            token += postfix[i];
            i++;
        }
        bool isNumber = !token.empty();
        for (size_t k = 0; k < token.size(); k++) {
            bool minusAtStart = (k == 0 && token[k] == '-' && token.size() > 1);
            if (!(token[k] >= '0' && token[k] <= '9') && !minusAtStart) {
                isNumber = false;
            }
        }
        if (isNumber) {
            stack.push_back(stoi(token));
            stackFrame(stack, false, token + " is a number: push it.", 1, ROLE_NEW, ROLE_NORMAL, info(token, ""));
        }
        else if (token.size() == 1 && isOperator(token[0])) {
            if (stack.size() < 2) {
                stackFrame(stack, false, "The operator " + token + " needs two operands but the stack has fewer. Invalid expression.", 2, ROLE_KEPT, ROLE_KEPT, info(token, ""));
                return false; // not enough operands
            }
            int b = stack.back();
            int a = stack[stack.size() - 2];
            stackFrame(stack, false, token + " is an operator: pop b = " + to_string(b) + " and then a = " + to_string(a) + ".", 2, ROLE_CURRENT, ROLE_CURRENT, info(token, ""));
            stack.pop_back();
            stack.pop_back();
            int value = 0;
            char op = token[0];
            if (op == '+') value = a + b;
            else if (op == '-') value = a - b;
            else if (op == '*') value = a * b;
            else if (op == '/') {
                if (b == 0) { stackFrame(stack, false, "Division by zero. Invalid expression.", 3, ROLE_KEPT, ROLE_NORMAL, info(token, "")); return false; }
                value = a / b;
            }
            else if (op == '%') {
                if (b == 0) { stackFrame(stack, false, "Division by zero. Invalid expression.", 3, ROLE_KEPT, ROLE_NORMAL, info(token, "")); return false; }
                value = a % b;
            }
            else {
                if (b < 0) return false;
                value = 1;
                for (int p = 0; p < b; p++) {
                    value *= a;
                }
            }
            stack.push_back(value);
            stackFrame(stack, false, to_string(a) + " " + op + " " + to_string(b) + " = " + to_string(value) + ". Push the result.", 3, ROLE_NEW, ROLE_NORMAL, info(token, to_string(a) + " " + op + " " + to_string(b) + " = " + to_string(value)));
        }
        else {
            stackFrame(stack, false, "A name or symbol (" + token + ") cannot be evaluated.", 0, ROLE_KEPT, ROLE_NORMAL, info(token, ""));
            return false; // a name or a symbol cannot be evaluated
        }
    }
    if (stack.size() != 1) {
        stackFrame(stack, false, "The expression ended with " + to_string(stack.size()) + " values on the stack instead of one. Invalid expression.", 4, ROLE_KEPT, ROLE_NORMAL, info("end", ""));
        return false;
    }
    result = stack.back();
    stackFrame(stack, false, "The only value left is the result: " + to_string(result) + ".", 4, ROLE_DONE, ROLE_NORMAL, info("end", "Result = " + to_string(result)));
    return true;
}
