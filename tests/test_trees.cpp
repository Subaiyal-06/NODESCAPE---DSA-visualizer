// Tests for the general binary tree and the BST

#include "TestUtils.h"
#include "../src/core/BinaryTree.h"
#include "../src/core/BST.h"
#include <algorithm>
#include <vector>
using namespace std;

static unsigned int seed = 777;
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
    int out[200];

    // general binary tree
    {
        BinaryTree t;
        check(t.isEmpty() && t.height() == 0 && t.size() == 0 && t.leafCount() == 0, "empty binary tree");
        check(t.preorder(out, 200) == 0 && t.levelOrder(out, 200) == 0 && !t.search(1), "traversals of an empty tree");
        for (int i = 1; i <= 7; i++) t.insertLevelOrder(i);
        check(t.size() == 7 && t.height() == 3 && t.leafCount() == 4, "size, height and leaves of a full tree");
        check(same(out, t.preorder(out, 200), {1, 2, 4, 5, 3, 6, 7}), "preorder");
        check(same(out, t.inorder(out, 200), {4, 2, 5, 1, 6, 3, 7}), "inorder");
        check(same(out, t.postorder(out, 200), {4, 5, 2, 6, 7, 3, 1}), "postorder");
        check(same(out, t.levelOrder(out, 200), {1, 2, 3, 4, 5, 6, 7}), "level order");
        check(t.search(6) && !t.search(9), "search");
        t.insertLevelOrder(8);
        check(t.height() == 4 && same(out, t.levelOrder(out, 200), {1, 2, 3, 4, 5, 6, 7, 8}), "next value goes to the first free spot");
        check(t.preorder(out, 3) == 3, "traversal respects maxCount");
        t.clear();
        check(t.isEmpty() && t.size() == 0, "clear");
        t.insertLevelOrder(5);
        check(t.height() == 1 && t.leafCount() == 1, "tree works after clear");
    }

    // BST
    {
        BST t;
        int v;
        check(t.isEmpty() && t.height() == 0 && !t.findMin(v) && !t.findMax(v) && !t.search(1) && !t.remove(1), "empty BST");
        int values[] = {50, 30, 70, 20, 40, 60, 80};
        for (int i = 0; i < 7; i++) check(t.insert(values[i]), "insert");
        check(!t.insert(40) && t.size() == 7, "duplicates are rejected");
        check(same(out, t.inorder(out, 200), {20, 30, 40, 50, 60, 70, 80}), "inorder is sorted");
        check(same(out, t.preorder(out, 200), {50, 30, 20, 40, 70, 60, 80}), "preorder");
        check(same(out, t.postorder(out, 200), {20, 40, 30, 60, 80, 70, 50}), "postorder");
        check(same(out, t.levelOrder(out, 200), {50, 30, 70, 20, 40, 60, 80}), "level order");
        check(t.height() == 3, "height");
        check(t.search(60) && t.lastComparisons() == 3, "search counts comparisons along the path");
        check(!t.search(65), "search miss");
        check(t.findMin(v) && v == 20 && t.findMax(v) && v == 80, "min and max");

        check(t.remove(20), "remove a leaf");
        check(same(out, t.inorder(out, 200), {30, 40, 50, 60, 70, 80}), "after removing a leaf");
        check(t.remove(30), "remove a node with one child");
        check(same(out, t.inorder(out, 200), {40, 50, 60, 70, 80}), "after removing a node with one child");
        check(t.remove(50), "remove the root, which has two children");
        check(same(out, t.preorder(out, 200), {60, 40, 70, 80}), "root replaced by its inorder successor");
        check(!t.remove(999) && t.size() == 4, "remove a missing value");
        while (t.size() > 0) {
            int top;
            t.preorder(&top, 1);
            t.remove(top);
        }
        check(t.isEmpty(), "remove everything");

        // random inserts and removes, checked against a plain sorted list
        BST big;
        vector<int> reference;
        bool consistent = true;
        for (int i = 0; i < 400; i++) {
            int value = nextRandom(150);
            bool present = find(reference.begin(), reference.end(), value) != reference.end();
            if (nextRandom(3) != 0) {
                bool inserted = big.insert(value);
                if (inserted == present) consistent = false;
                if (!present) reference.push_back(value);
            }
            else {
                bool removed = big.remove(value);
                if (removed != present) consistent = false;
                if (present) reference.erase(find(reference.begin(), reference.end(), value));
            }
        }
        sort(reference.begin(), reference.end());
        int n = big.inorder(out, 200);
        check(consistent && same(out, n, reference) && big.size() == (int)reference.size(), "random inserts and removes stay consistent");
    }

    return finishTests();
}
