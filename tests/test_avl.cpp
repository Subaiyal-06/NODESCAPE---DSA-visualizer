// Tests for the AVL tree. After every change the whole tree is checked with isValidAVL().

#include "TestUtils.h"
#include "../src/core/AVLTree.h"
#include <vector>
using namespace std;

static unsigned int seed = 4242;
static int nextRandom(int limit) {
    seed = seed * 1103515245u + 12345u;
    return (int)((seed / 65536u) % (unsigned int)limit);
}

static bool same(const int a[], int n, const vector<int>& expected) {
    if (n != (int)expected.size()) return false;
    for (int i = 0; i < n; i++) {
        if (a[i] != expected[i]) return false;
    }
    return true;
}

int main() {
    int out[300];

    // the four rotation cases
    {
        AVLTree rr;
        rr.insert(10); rr.insert(20); rr.insert(30);
        check(same(out, rr.preorder(out, 300), {20, 10, 30}) && rr.rotationCount() == 1, "right-right case (left rotation)");

        AVLTree ll;
        ll.insert(30); ll.insert(20); ll.insert(10);
        check(same(out, ll.preorder(out, 300), {20, 10, 30}) && ll.rotationCount() == 1, "left-left case (right rotation)");

        AVLTree lr;
        lr.insert(30); lr.insert(10); lr.insert(20);
        check(same(out, lr.preorder(out, 300), {20, 10, 30}) && lr.rotationCount() == 2, "left-right case (two rotations)");

        AVLTree rl;
        rl.insert(10); rl.insert(30); rl.insert(20);
        check(same(out, rl.preorder(out, 300), {20, 10, 30}) && rl.rotationCount() == 2, "right-left case (two rotations)");
    }

    // basic behaviour
    {
        AVLTree t;
        check(t.isEmpty() && t.height() == 0 && t.isValidAVL() && !t.search(1) && !t.remove(1), "empty AVL tree");
        check(t.insert(5) && !t.insert(5) && t.size() == 1, "duplicates are rejected");
        check(t.search(5) && !t.search(6), "search");
        check(t.remove(5) && t.isEmpty(), "remove the only node");
        t.insert(1);
        t.clear();
        check(t.isEmpty() && t.size() == 0, "clear");
    }

    // ascending inserts give a perfectly balanced tree
    {
        AVLTree t;
        for (int i = 1; i <= 127; i++) t.insert(i);
        check(t.size() == 127 && t.height() == 7 && t.isValidAVL(), "1 to 127 in order gives height 7");
        int n = t.inorder(out, 300);
        bool sorted = (n == 127);
        for (int i = 0; i < n && sorted; i++) {
            if (out[i] != i + 1) sorted = false;
        }
        check(sorted, "inorder is sorted");
    }

    // deleting forces rotations too
    {
        AVLTree t;
        int values[] = {50, 30, 70, 20, 40, 60, 80, 10};
        for (int i = 0; i < 8; i++) t.insert(values[i]);
        check(t.remove(60) && t.remove(80) && t.isValidAVL(), "deleting on one side keeps the tree balanced");
        check(t.rotationCount() >= 1, "a rotation was needed after deleting");
        check(t.remove(70) && t.isValidAVL(), "delete a node with a child");
        check(t.remove(50) && t.isValidAVL() && t.size() == 4, "delete the root");
    }

    // random inserts and removes
    {
        AVLTree t;
        bool present[200];
        for (int i = 0; i < 200; i++) present[i] = false;
        int expectedSize = 0;
        bool allValid = true;
        bool resultsRight = true;
        for (int i = 0; i < 1500; i++) {
            int value = nextRandom(200);
            if (nextRandom(5) < 3) {
                bool inserted = t.insert(value);
                if (inserted == present[value]) resultsRight = false;
                if (!present[value]) { present[value] = true; expectedSize++; }
            }
            else {
                bool removed = t.remove(value);
                if (removed != present[value]) resultsRight = false;
                if (present[value]) { present[value] = false; expectedSize--; }
            }
            if (!t.isValidAVL() || t.size() != expectedSize) allValid = false;
        }
        check(resultsRight, "insert and remove report the right result");
        check(allValid, "tree is a valid AVL tree after each of 1500 random operations");
        bool searchOk = true;
        for (int v = 0; v < 200; v++) {
            if (t.search(v) != present[v]) searchOk = false;
        }
        check(searchOk, "search agrees with the reference");
    }

    return finishTests();
}
