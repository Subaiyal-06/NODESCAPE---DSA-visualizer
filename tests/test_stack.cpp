// Tests for both stacks, the stack applications and recursion

#include "TestUtils.h"
#include "../src/core/ArrayStack.h"
#include "../src/core/LinkedStack.h"
#include "../src/core/StackApps.h"
#include "../src/core/Recursion.h"
#include <string>
using namespace std;

int main() {
    // array stack
    {
        ArrayStack s;
        int v;
        check(s.isEmpty() && !s.pop(v) && !s.peek(v), "empty array stack");
        for (int i = 1; i <= ArrayStack::MAX; i++) check(s.push(i * 10), "push until full");
        check(s.isFull() && !s.push(999), "overflow is reported");
        check(s.size() == ArrayStack::MAX, "size when full");
        check(s.peek(v) && v == 80, "peek shows the top");
        check(s.pop(v) && v == 80 && s.pop(v) && v == 70, "last in, first out");
        check(s.push(5) && s.peek(v) && v == 5, "push after pop");
        s.clear();
        check(s.isEmpty(), "clear");
    }

    // linked stack
    {
        LinkedStack s;
        int v;
        check(s.isEmpty() && !s.pop(v) && !s.peek(v), "empty linked stack");
        s.push(1); s.push(2); s.push(3);
        check(s.size() == 3 && s.peek(v) && v == 3, "linked stack top");
        check(s.pop(v) && v == 3 && s.pop(v) && v == 2 && s.pop(v) && v == 1, "linked stack order");
        check(!s.pop(v) && s.isEmpty(), "linked stack underflow");
        s.push(7);
        s.clear();
        check(s.isEmpty() && s.size() == 0, "linked stack clear");
    }

    // balanced brackets
    {
        check(isBalanced("{[()]}"), "nested brackets");
        check(isBalanced("a(b)c[d]{e}"), "brackets with letters");
        check(isBalanced(""), "empty text");
        check(!isBalanced("([)]"), "wrong order");
        check(!isBalanced("(("), "never closed");
        check(!isBalanced("))"), "never opened");
        check(!isBalanced(")("), "closing before opening");
    }

    // infix to postfix
    {
        bool ok;
        check(infixToPostfix("a+b*c", ok) == "a b c * +" && ok, "precedence");
        check(infixToPostfix("(a+b)*c", ok) == "a b + c *" && ok, "brackets");
        check(infixToPostfix("3+4*2/(1-5)^2^3", ok) == "3 4 2 * 1 5 - 2 3 ^ ^ / +" && ok, "classic example, ^ goes right to left");
        check(infixToPostfix("12 + 345", ok) == "12 345 +" && ok, "multi digit numbers");
        check(infixToPostfix("a-b-c", ok) == "a b - c -" && ok, "left to right for equal precedence");
        infixToPostfix("(a+b", ok);
        check(!ok, "missing closing bracket");
        infixToPostfix("a+b)", ok);
        check(!ok, "missing opening bracket");
        infixToPostfix("a$b", ok);
        check(!ok, "unknown character");
    }

    // postfix evaluation
    {
        int r;
        check(evaluatePostfix("3 4 2 * +", r) && r == 11, "simple postfix");
        check(evaluatePostfix("5 1 2 + 4 * + 3 -", r) && r == 14, "longer postfix");
        check(evaluatePostfix("2 3 ^", r) && r == 8, "power");
        check(evaluatePostfix("-3 4 +", r) && r == 1, "negative number");
        check(evaluatePostfix("17 5 %", r) && r == 2, "remainder");
        check(!evaluatePostfix("10 0 /", r), "division by zero");
        check(!evaluatePostfix("1 +", r), "not enough operands");
        check(!evaluatePostfix("1 2", r), "too many operands");
        check(!evaluatePostfix("a b +", r), "names cannot be evaluated");
        check(!evaluatePostfix("", r), "empty expression");
        bool ok;
        string postfix = infixToPostfix("(8-3)*(2+4)", ok);
        check(ok && evaluatePostfix(postfix, r) && r == 30, "infix to postfix to value");
    }

    // recursion
    {
        long long calls = 0;
        check(factorial(5) == 120 && factorial(0) == 1 && factorial(1) == 1, "factorial");
        check(factorial(20) == 2432902008176640000LL, "largest factorial that fits");
        check(factorial(21) == -1 && factorial(-1) == -1, "factorial limits");
        check(fibonacci(10, calls) == 55 && calls == 177, "fibonacci and its call count");
        check(power(2, 10) == 1024 && power(5, 0) == 1 && power(3, -1) == -1, "power");
        check(gcd(48, 18) == 6 && gcd(0, 5) == 5 && gcd(7, 0) == 7, "gcd");
        check(sumOfDigits(12345) == 15 && sumOfDigits(0) == 0, "sum of digits");
        check(hanoi(3, 'A', 'C', 'B', false) == 7 && hanoi(5, 'A', 'C', 'B', false) == 31 && hanoi(0, 'A', 'C', 'B', false) == 0, "tower of hanoi moves");
    }

    return finishTests();
}
