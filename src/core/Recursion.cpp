#include "Recursion.h"
#include <iostream>
using namespace std;

long long factorial(int n) {
    if (n < 0 || n > 20) {
        return -1;
    }
    if (n <= 1) {
        return 1; // base case
    }
    return n * factorial(n - 1);
}

long long factorialTraced(int n, int depth) {
    string indent(depth * 3, ' ');
    cout << indent << "factorial(" << n << ") called" << endl;
    long long result;
    if (n <= 1) {
        result = 1;
        cout << indent << "base case reached, returns 1" << endl;
    }
    else {
        result = n * factorialTraced(n - 1, depth + 1);
        cout << indent << "factorial(" << n << ") returns " << result << endl;
    }
    return result;
}

long long fibonacci(int n, long long& calls) {
    calls++;
    if (n <= 1) {
        return n;
    }
    return fibonacci(n - 1, calls) + fibonacci(n - 2, calls);
}

long long power(int base, int exponent) {
    if (exponent < 0) {
        return -1;
    }
    if (exponent == 0) {
        return 1;
    }
    return base * power(base, exponent - 1);
}

int gcd(int a, int b) {
    if (a < 0) a = -a;
    if (b < 0) b = -b;
    if (b == 0) {
        return a;
    }
    return gcd(b, a % b);
}

int sumOfDigits(int n) {
    if (n < 0) n = -n;
    if (n < 10) {
        return n;
    }
    return (n % 10) + sumOfDigits(n / 10);
}

int hanoi(int disks, char from, char to, char spare, bool print) {
    if (disks <= 0) {
        return 0;
    }
    int moves = hanoi(disks - 1, from, spare, to, print); // move the smaller tower out of the way
    if (print) {
        cout << "Move disk " << disks << " from " << from << " to " << to << endl;
    }
    moves++;
    moves += hanoi(disks - 1, spare, to, from, print);    // put the smaller tower on top again
    return moves;
}

// ---------- recorded versions (for the graphical app) ----------

#include "Recorder.h"
#include <vector>

namespace {
    const vector<string> CODE_FACT = {
        "long long factorial(int n) {",
        "  if (n <= 1) return 1;            // base case",
        "  return n * factorial(n - 1);     // the call waits for the smaller one",
        "}"
    };
    const vector<string> CODE_FIB = {
        "int fib(int n) {",
        "  if (n <= 1) return n;            // base case",
        "  return fib(n - 1) + fib(n - 2);  // two calls: the work doubles",
        "}"
    };
    const vector<string> CODE_HANOI = {
        "hanoi(n, from, to, spare) {",
        "  if (n == 0) return;",
        "  hanoi(n - 1, from, spare, to);   // clear the way",
        "  move disk n from -> to;",
        "  hanoi(n - 1, spare, to, from);   // put them back on top",
        "}"
    };

    // ----- factorial: the call stack -----
    void factFrame(const vector<int>& st, const string& msg, int line, Role top, const vector<string>& info) {
        if (!Recorder::enabled()) return;
        Frame f;
        f.kind = KIND_STACK;
        f.message = msg;
        f.codeLine = line;
        f.auxLabel = "calls";
        for (size_t i = 0; i < st.size(); i++) f.cells.push_back(Cell(st[i], i + 1 == st.size() ? top : ROLE_NORMAL));
        if (!st.empty()) f.markers.push_back(Marker((int)st.size() - 1, "running"));
        f.info = info;
        Recorder::add(f);
    }

    long long factRec(int n, vector<int>& st, vector<string>& log) {
        st.push_back(n);
        factFrame(st, "factorial(" + to_string(n) + ") is called. It goes on the call stack.", 0, ROLE_NEW, log);
        long long result;
        if (n <= 1) {
            result = 1;
            factFrame(st, "Base case: factorial(" + to_string(n) + ") returns 1 at once.", 1, ROLE_FOUND, log);
        }
        else {
            long long inner = factRec(n - 1, st, log);
            result = n * inner;
            factFrame(st, "Back in factorial(" + to_string(n) + "): " + to_string(n) + " * " + to_string(inner) + " = " + to_string(result) + ".", 2, ROLE_CURRENT, log);
        }
        log.push_back("factorial(" + to_string(n) + ") returned " + to_string(result));
        st.pop_back();
        factFrame(st, "factorial(" + to_string(n) + ") returns " + to_string(result) + " and leaves the stack.", 3, ROLE_CURRENT, log);
        return result;
    }

    // ----- fibonacci: the call tree -----
    struct FibNode {
        int id, n, left, right;
        long long result;
        bool done;
        Role role;
    };

    void fibFrame(const vector<FibNode>& nodes, const string& msg, int line, long long calls) {
        if (!Recorder::enabled()) return;
        Frame f;
        f.kind = KIND_TREE;
        f.message = msg;
        f.codeLine = line;
        f.comparisons = calls;
        f.rootId = nodes.empty() ? -1 : 0;
        for (size_t i = 0; i < nodes.size(); i++) {
            TreeNodeInfo t;
            t.id = nodes[i].id;
            t.value = nodes[i].n;
            t.left = nodes[i].left;
            t.right = nodes[i].right;
            t.role = nodes[i].role;
            t.hasAux = nodes[i].done;
            t.aux = (int)nodes[i].result;
            f.nodes.push_back(t);
        }
        f.auxLabel = "fib";
        Recorder::add(f);
    }

    long long fibRec(int n, int parent, bool isLeft, vector<FibNode>& nodes, long long& calls) {
        int id = (int)nodes.size();
        FibNode node;
        node.id = id;
        node.n = n;
        node.left = -1;
        node.right = -1;
        node.result = 0;
        node.done = false;
        node.role = ROLE_NEW;
        nodes.push_back(node);
        if (parent >= 0) {
            if (isLeft) nodes[parent].left = id;
            else nodes[parent].right = id;
        }
        calls++;
        fibFrame(nodes, "fib(" + to_string(n) + ") is called (call number " + to_string(calls) + ").", 0, calls);
        nodes[id].role = ROLE_CURRENT;
        long long result;
        if (n <= 1) {
            result = n;
            nodes[id].result = result;
            nodes[id].done = true;
            nodes[id].role = ROLE_FOUND;
            fibFrame(nodes, "Base case: fib(" + to_string(n) + ") = " + to_string(n) + ".", 1, calls);
        }
        else {
            long long a = fibRec(n - 1, id, true, nodes, calls);
            nodes[id].role = ROLE_CURRENT;
            long long b = fibRec(n - 2, id, false, nodes, calls);
            result = a + b;
            nodes[id].result = result;
            nodes[id].done = true;
            nodes[id].role = ROLE_DONE;
            nodes[id].role = ROLE_NEW;
            fibFrame(nodes, "fib(" + to_string(n) + ") = fib(" + to_string(n - 1) + ") + fib(" + to_string(n - 2) + ") = " + to_string(a) + " + " + to_string(b) + " = " + to_string(result) + ".", 2, calls);
        }
        nodes[id].role = ROLE_DONE;
        return result;
    }

    // ----- towers of hanoi -----
    struct Pegs {
        vector<int> peg[3];
    };

    void hanoiFrame(const Pegs& p, const string& msg, int line, int movingFrom, int movingTo, bool moved, long moves) {
        if (!Recorder::enabled()) return;
        Frame f;
        f.kind = KIND_HANOI;
        f.message = msg;
        f.codeLine = line;
        f.swaps = moves;
        for (int k = 0; k < 3; k++) {
            vector<Cell> pile;
            for (size_t i = 0; i < p.peg[k].size(); i++) {
                Role r = ROLE_NORMAL;
                if (i + 1 == p.peg[k].size()) {
                    if (!moved && k == movingFrom) r = ROLE_CURRENT;
                    if (moved && k == movingTo) r = ROLE_NEW;
                }
                pile.push_back(Cell(p.peg[k][i], r));
            }
            f.chains.push_back(pile);
        }
        Recorder::add(f);
    }

    int hanoiRec(int n, int from, int to, int spare, Pegs& p, long& moves) {
        if (n <= 0) return 0;
        int count = hanoiRec(n - 1, from, spare, to, p, moves);
        const char names[3] = { 'A', 'B', 'C' };
        moves++;
        hanoiFrame(p, "Move disk " + to_string(n) + " from " + string(1, names[from]) + " to " + string(1, names[to]) + ".", 3, from, to, false, moves - 1);
        p.peg[from].pop_back();
        p.peg[to].push_back(n);
        hanoiFrame(p, "Disk " + to_string(n) + " is now on peg " + string(1, names[to]) + " (move " + to_string(moves) + ").", 3, from, to, true, moves);
        count++;
        count += hanoiRec(n - 1, spare, to, from, p, moves);
        return count;
    }
}

long long factorialRecorded(int n) {
    Recorder::begin("Factorial (call stack)", CODE_FACT);
    if (n < 0 || n > 12) {
        factFrame(vector<int>(), "Choose n between 0 and 12 for this picture.", 0, ROLE_KEPT, vector<string>());
        return -1;
    }
    vector<int> st;
    vector<string> log;
    long long r = factRec(n, st, log);
    log.push_back("Result: " + to_string(n) + "! = " + to_string(r));
    factFrame(st, "The stack is empty again. " + to_string(n) + "! = " + to_string(r) + ".", -1, ROLE_DONE, log);
    return r;
}

long long fibonacciRecorded(int n) {
    Recorder::begin("Fibonacci (call tree)", CODE_FIB);
    if (n < 0 || n > 6) {
        fibFrame(vector<FibNode>(), "Choose n between 0 and 6 for this picture (the tree doubles with every step).", 0, 0);
        return -1;
    }
    vector<FibNode> nodes;
    long long calls = 0;
    long long r = fibRec(n, -1, true, nodes, calls);
    fibFrame(nodes, "fib(" + to_string(n) + ") = " + to_string(r) + " needed " + to_string(calls) + " calls. The same values are computed again and again.", -1, calls);
    return r;
}

int hanoiRecorded(int disks) {
    Recorder::begin("Towers of Hanoi", CODE_HANOI);
    if (disks < 1 || disks > 5) {
        hanoiFrame(Pegs(), "Choose 1 to 5 disks for this picture.", 0, -1, -1, false, 0);
        return 0;
    }
    Pegs p;
    for (int d = disks; d >= 1; d--) p.peg[0].push_back(d);
    hanoiFrame(p, "Move " + to_string(disks) + " disks from peg A to peg C using peg B. A bigger disk never goes on a smaller one.", 0, -1, -1, false, 0);
    long moves = 0;
    int count = hanoiRec(disks, 0, 2, 1, p, moves);
    hanoiFrame(p, "Done in " + to_string(count) + " moves. That is 2^" + to_string(disks) + " - 1.", -1, -1, -1, false, moves);
    return count;
}
