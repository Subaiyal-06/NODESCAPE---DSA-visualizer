#include "AVLTree.h"
#include "Trace.h"
#include "Recorder.h"
#include <iostream>
#include <string>
#include <vector>
using namespace std;

// ---------- recording helpers (used only when the recorder is switched on) ----------

namespace {
    const vector<string> CODE_AVL_INSERT = {
        "Node* insert(Node* n, int v) {",
        "  if (n == NULL) return new Node(v);",
        "  if (v < n->data) n->left = insert(n->left, v);",
        "  else if (v > n->data) n->right = insert(n->right, v);",
        "  bf = height(n->left) - height(n->right);",
        "  if (bf > 1)  rotate right (LL), or left then right (LR)",
        "  if (bf < -1) rotate left (RR), or right then left (RL)",
        "  return n; }"
    };
    const vector<string> CODE_AVL_REMOVE = {
        "Node* remove(Node* n, int v) {",
        "  if (n == NULL) return NULL;",
        "  if (v < n->data) n->left = remove(n->left, v);",
        "  else if (v > n->data) n->right = remove(n->right, v);",
        "  else if (one child or none) return the child;",
        "  else { copy the smallest right value here; remove it from the right }",
        "  bf = height(n->left) - height(n->right);",
        "  if (bf > 1 or bf < -1) rotate to restore balance",
        "  return n; }"
    };

    // State of the recording while one insert or remove runs. A rotation is only linked into the
    // tree when the recursion returns to the parent, so a picture taken in between needs to know
    // that "old" is really "replacement".
    struct Ctx {
        AVLNode* root;
        AVLNode* ovOld;
        AVLNode* ovNew;
        long visited;
        int justAdded; // id of a node that was just created (green in the next picture), -1 for none
        Ctx() : root(nullptr), ovOld(nullptr), ovNew(nullptr), visited(0), justAdded(-1) {}
    };
    Ctx ctx;

    // The override only replaces the parent's old link to a rotated node. Inside the rotated
    // subtree (where the old node is now a child of the new top) it must not apply again.
    AVLNode* resolve(AVLNode* node, bool& active) {
        if (active && node != nullptr && node == ctx.ovOld) {
            active = false;
            return ctx.ovNew;
        }
        return node;
    }

    int realHeight(AVLNode* node, bool active) {
        node = resolve(node, active);
        if (node == nullptr) return 0;
        bool al = active, ar = active;
        int l = realHeight(resolve(node->left, al), al);
        int r = realHeight(resolve(node->right, ar), ar);
        return 1 + (l > r ? l : r);
    }

    struct Mark {
        int cur, bad, g1, g2, g3, removed;
        Mark() : cur(-1), bad(-1), g1(-1), g2(-1), g3(-1), removed(-1) {}
    };

    void collect(AVLNode* node, bool active, const Mark& m, vector<TreeNodeInfo>& out) {
        node = resolve(node, active);
        if (node == nullptr) return;
        bool al = active, ar = active;
        AVLNode* l = resolve(node->left, al);
        AVLNode* r = resolve(node->right, ar);
        TreeNodeInfo info;
        info.id = node->id;
        info.value = node->data;
        info.left = l != nullptr ? l->id : -1;
        info.right = r != nullptr ? r->id : -1;
        info.hasAux = true;
        info.aux = realHeight(l, al) - realHeight(r, ar); // balance factor
        if (node->id == ctx.justAdded) info.role = ROLE_NEW;
        if (node->id == m.g1 || node->id == m.g2 || node->id == m.g3) info.role = ROLE_NEW;
        if (node->id == m.cur) info.role = ROLE_CURRENT;
        if (node->id == m.bad) info.role = ROLE_KEPT;
        if (node->id == m.removed) info.role = ROLE_REMOVED;
        out.push_back(info);
        collect(l, al, m, out);
        collect(r, ar, m, out);
    }

    void snap(const string& message, int line, const Mark& m) {
        if (!Recorder::enabled()) return;
        Frame f;
        f.kind = KIND_TREE;
        f.message = message;
        f.codeLine = line;
        f.comparisons = ctx.visited;
        bool active = true;
        AVLNode* r = resolve(ctx.root, active);
        f.rootId = r != nullptr ? r->id : -1;
        collect(ctx.root, true, m, f.nodes);
        Recorder::add(f);
    }
}

AVLTree::AVLTree() {
    root = nullptr;
    count = 0;
    rotations = 0;
    nextId = 1;
}

AVLTree::~AVLTree() {
    clear();
}

bool AVLTree::isEmpty() const {
    return root == nullptr;
}

int AVLTree::size() const {
    return count;
}

int AVLTree::height() const {
    return heightOf(root);
}

int AVLTree::rotationCount() const {
    return rotations;
}

void AVLTree::destroy(AVLNode* node) {
    if (node == nullptr) {
        return;
    }
    destroy(node->left);
    destroy(node->right);
    delete node;
}

void AVLTree::clear() {
    destroy(root);
    root = nullptr;
    count = 0;
}

int AVLTree::heightOf(AVLNode* node) const {
    return node == nullptr ? 0 : node->height;
}

int AVLTree::balanceFactor(AVLNode* node) const {
    if (node == nullptr) {
        return 0;
    }
    return heightOf(node->left) - heightOf(node->right); // positive means left heavy
}

void AVLTree::updateHeight(AVLNode* node) {
    int leftHeight = heightOf(node->left);
    int rightHeight = heightOf(node->right);
    node->height = 1 + (leftHeight > rightHeight ? leftHeight : rightHeight);
}

// Right rotation: the left child x of y moves up and y becomes the right child of x.
// The right subtree of x (B) is handed over to y as its new left subtree.
// A left rotation is the mirror image of this.
AVLNode* AVLTree::rotateRight(AVLNode* y) {
    AVLNode* x = y->left;
    y->left = x->right;
    x->right = y;
    updateHeight(y); // y is lower now, so it is updated first
    updateHeight(x);
    rotations++;
    Trace::say("right rotation around " + to_string(y->data) + ", " + to_string(x->data) + " moves up");
    return x;
}

AVLNode* AVLTree::rotateLeft(AVLNode* x) {
    AVLNode* y = x->right;
    x->right = y->left;
    y->left = x;
    updateHeight(x);
    updateHeight(y);
    rotations++;
    Trace::say("left rotation around " + to_string(x->data) + ", " + to_string(y->data) + " moves up");
    return y;
}

AVLNode* AVLTree::rebalance(AVLNode* node) {
    updateHeight(node);
    int balance = balanceFactor(node);

    if (Recorder::enabled()) {
        ctx.visited++;
        Mark m;
        if (balance > 1 || balance < -1) {
            m.bad = node->id;
            snap("Back at " + to_string(node->data) + ": balance factor = " + to_string(balance) + ". Too " + (balance > 0 ? "left" : "right") + " heavy, a rotation is needed.", balance > 0 ? 5 : 6, m);
        }
        else {
            m.cur = node->id;
            snap("Back at " + to_string(node->data) + ": balance factor = " + to_string(balance) + ". Balanced, nothing to do.", 4, m);
        }
    }

    if (balance > 1) { // left heavy
        AVLNode* x = node->left;
        if (balanceFactor(node->left) < 0) {
            Trace::say("imbalance at " + to_string(node->data) + ": left-right case");
            AVLNode* mid = node->left->right;
            node->left = rotateLeft(node->left);
            if (Recorder::enabled()) {
                Mark m;
                m.g1 = mid->id;
                m.g2 = x->id;
                m.bad = node->id;
                snap("Left-Right case. First rotate left around " + to_string(x->data) + " so the heavy side is a straight line.", 5, m);
            }
            x = node->left;
        }
        else {
            Trace::say("imbalance at " + to_string(node->data) + ": left-left case");
        }
        AVLNode* top = rotateRight(node);
        if (Recorder::enabled()) {
            ctx.ovOld = node;
            ctx.ovNew = top;
            Mark m;
            m.g1 = top->id;
            m.g2 = node->id;
            snap("Rotate right around " + to_string(node->data) + ": " + to_string(top->data) + " moves up and " + to_string(node->data) + " moves down. Balance restored.", 5, m);
            ctx.ovOld = nullptr;
            ctx.ovNew = nullptr;
        }
        return top;
    }
    if (balance < -1) { // right heavy
        AVLNode* y = node->right;
        if (balanceFactor(node->right) > 0) {
            Trace::say("imbalance at " + to_string(node->data) + ": right-left case");
            AVLNode* mid = node->right->left;
            node->right = rotateRight(node->right);
            if (Recorder::enabled()) {
                Mark m;
                m.g1 = mid->id;
                m.g2 = y->id;
                m.bad = node->id;
                snap("Right-Left case. First rotate right around " + to_string(y->data) + " so the heavy side is a straight line.", 6, m);
            }
        }
        else {
            Trace::say("imbalance at " + to_string(node->data) + ": right-right case");
        }
        AVLNode* top = rotateLeft(node);
        if (Recorder::enabled()) {
            ctx.ovOld = node;
            ctx.ovNew = top;
            Mark m;
            m.g1 = top->id;
            m.g2 = node->id;
            snap("Rotate left around " + to_string(node->data) + ": " + to_string(top->data) + " moves up and " + to_string(node->data) + " moves down. Balance restored.", 6, m);
            ctx.ovOld = nullptr;
            ctx.ovNew = nullptr;
        }
        return top;
    }
    return node;
}

AVLNode* AVLTree::insertInto(AVLNode* node, int value, bool& inserted) {
    if (node == nullptr) {
        inserted = true;
        Trace::say("empty spot found, " + to_string(value) + " inserted");
        AVLNode* created = new AVLNode(value, nextId++);
        ctx.justAdded = created->id;
        return created;
    }
    ctx.visited++;
    if (value < node->data) {
        if (Recorder::enabled()) {
            Mark m;
            m.cur = node->id;
            snap(to_string(value) + " < " + to_string(node->data) + ", go left.", 2, m);
        }
        node->left = insertInto(node->left, value, inserted);
    }
    else if (value > node->data) {
        if (Recorder::enabled()) {
            Mark m;
            m.cur = node->id;
            snap(to_string(value) + " > " + to_string(node->data) + ", go right.", 3, m);
        }
        node->right = insertInto(node->right, value, inserted);
    }
    else {
        if (Recorder::enabled()) {
            Mark m;
            m.cur = node->id;
            snap(to_string(value) + " is already in the tree. Duplicates are not allowed.", 3, m);
        }
        return node; // duplicate
    }
    if (ctx.justAdded >= 0) {
        // the new node is linked now, show it before any balance check
        snap("Reached an empty spot. " + to_string(value) + " is inserted as a new leaf.", 1, Mark());
        ctx.justAdded = -1;
    }
    return rebalance(node); // check the balance on the way back up
}

AVLNode* AVLTree::removeFrom(AVLNode* node, int key, bool& removed) {
    if (node == nullptr) {
        if (Recorder::enabled()) {
            snap("Reached NULL. " + to_string(key) + " is not in the tree.", 1, Mark());
        }
        return nullptr;
    }
    ctx.visited++;
    if (key < node->data) {
        if (Recorder::enabled()) {
            Mark m;
            m.cur = node->id;
            snap(to_string(key) + " < " + to_string(node->data) + ", go left.", 2, m);
        }
        node->left = removeFrom(node->left, key, removed);
    }
    else if (key > node->data) {
        if (Recorder::enabled()) {
            Mark m;
            m.cur = node->id;
            snap(to_string(key) + " > " + to_string(node->data) + ", go right.", 3, m);
        }
        node->right = removeFrom(node->right, key, removed);
    }
    else {
        removed = true;
        if (Recorder::enabled()) {
            Mark m;
            m.removed = node->id;
            snap("Found " + to_string(key) + ". This node will be removed.", 4, m);
        }
        if (node->left == nullptr || node->right == nullptr) {
            AVLNode* child = (node->left != nullptr) ? node->left : node->right;
            Trace::say("removed " + to_string(key));
            if (Recorder::enabled()) {
                ctx.ovOld = node;
                ctx.ovNew = child;
                snap("It has at most one child, so the child takes its place.", 4, Mark());
                ctx.ovOld = nullptr;
                ctx.ovNew = nullptr;
            }
            delete node;
            return child; // can be NULL when the node was a leaf
        }
        AVLNode* successor = node->right;
        while (successor->left != nullptr) {
            successor = successor->left;
        }
        Trace::say(to_string(key) + " has two children, replaced by " + to_string(successor->data));
        if (Recorder::enabled()) {
            Mark m;
            m.removed = node->id;
            m.cur = successor->id;
            snap("Two children: the smallest value of the right side (" + to_string(successor->data) + ") will replace " + to_string(key) + ".", 5, m);
        }
        node->data = successor->data;
        bool ignored = false;
        node->right = removeFrom(node->right, successor->data, ignored);
    }
    return rebalance(node);
}

bool AVLTree::insert(int value) {
    Recorder::begin("Insert into the AVL tree", CODE_AVL_INSERT);
    ctx = Ctx();
    ctx.root = root;
    bool inserted = false;
    if (root == nullptr) {
        root = insertInto(root, value, inserted);
        ctx.root = root;
        ctx.justAdded = root->id;
        snap("The tree is empty, so " + to_string(value) + " becomes the root.", 1, Mark());
        ctx.justAdded = -1;
    }
    else {
        root = insertInto(root, value, inserted);
        ctx.root = root;
        ctx.justAdded = -1;
        if (inserted) {
            snap("Insert complete. Every balance factor is -1, 0 or 1 again (" + to_string(rotations) + " rotations so far).", -1, Mark());
        }
    }
    if (inserted) {
        count++;
    }
    return inserted;
}

bool AVLTree::remove(int key) {
    Recorder::begin("Delete from the AVL tree", CODE_AVL_REMOVE);
    ctx = Ctx();
    ctx.root = root;
    bool removed = false;
    root = removeFrom(root, key, removed);
    ctx.root = root;
    if (removed) {
        count--;
        snap("Delete complete. The tree is balanced again (" + to_string(rotations) + " rotations so far).", -1, Mark());
    }
    return removed;
}

bool AVLTree::search(int key) const {
    AVLNode* current = root;
    while (current != nullptr) {
        if (key == current->data) {
            return true;
        }
        current = (key < current->data) ? current->left : current->right;
    }
    return false;
}

void AVLTree::inorderFrom(AVLNode* node, int out[], int& index, int maxCount) const {
    if (node == nullptr) {
        return;
    }
    inorderFrom(node->left, out, index, maxCount);
    if (index < maxCount) {
        out[index++] = node->data;
    }
    inorderFrom(node->right, out, index, maxCount);
}

void AVLTree::preorderFrom(AVLNode* node, int out[], int& index, int maxCount) const {
    if (node == nullptr || index >= maxCount) {
        return;
    }
    out[index++] = node->data;
    preorderFrom(node->left, out, index, maxCount);
    preorderFrom(node->right, out, index, maxCount);
}

int AVLTree::inorder(int out[], int maxCount) const {
    int index = 0;
    inorderFrom(root, out, index, maxCount);
    return index;
}

int AVLTree::preorder(int out[], int maxCount) const {
    int index = 0;
    preorderFrom(root, out, index, maxCount);
    return index;
}

bool AVLTree::checkNode(AVLNode* node, long long low, long long high, int& height) const {
    if (node == nullptr) {
        height = 0;
        return true;
    }
    if (node->data <= low || node->data >= high) {
        return false; // BST order is broken
    }
    int leftHeight, rightHeight;
    if (!checkNode(node->left, low, node->data, leftHeight)) return false;
    if (!checkNode(node->right, node->data, high, rightHeight)) return false;
    height = 1 + (leftHeight > rightHeight ? leftHeight : rightHeight);
    if (node->height != height) {
        return false; // stored height is wrong
    }
    int diff = leftHeight - rightHeight;
    return diff >= -1 && diff <= 1;
}

bool AVLTree::isValidAVL() const {
    int height;
    return checkNode(root, -4000000000LL, 4000000000LL, height);
}

void AVLTree::printSideways(AVLNode* node, int indent) const {
    if (node == nullptr) {
        return;
    }
    printSideways(node->right, indent + 8);
    cout << string(indent, ' ') << node->data << " (bf " << balanceFactor(node) << ")" << endl;
    printSideways(node->left, indent + 8);
}

void AVLTree::displayTree() const {
    if (root == nullptr) {
        cout << "Tree is empty." << endl;
        return;
    }
    printSideways(root, 0);
}
