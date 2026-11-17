#include "BST.h"
#include "Trace.h"
#include "Recorder.h"
#include <iostream>
#include <string>
using namespace std;

// ---------- recording helpers (used only when the recorder is switched on) ----------

namespace {
    const vector<string> CODE_INSERT = {
        "Node* insert(Node* r, int v) {",
        "  if (r == NULL) return new Node(v);",
        "  if (v < r->data) r->left = insert(r->left, v);",
        "  else if (v > r->data) r->right = insert(r->right, v);",
        "  return r; }"
    };
    const vector<string> CODE_SEARCH = {
        "bool search(Node* r, int v) {",
        "  if (r == NULL) return false;",
        "  if (v == r->data) return true;",
        "  if (v < r->data) return search(r->left, v);",
        "  return search(r->right, v); }"
    };
    const vector<string> CODE_REMOVE = {
        "Node* remove(Node* r, int v) {",
        "  if (r == NULL) return NULL;",
        "  if (v < r->data) r->left = remove(r->left, v);",
        "  else if (v > r->data) r->right = remove(r->right, v);",
        "  else {",
        "    if (!r->left || !r->right) {      // 0 or 1 child",
        "      Node* c = r->left ? r->left : r->right;",
        "      delete r; return c; }",
        "    Node* s = minNode(r->right);      // 2 children",
        "    r->data = s->data;",
        "    r->right = remove(r->right, s->data); }",
        "  return r; }"
    };
    const vector<string> CODE_PREORDER = {
        "void preorder(Node* r) {",
        "  if (r == NULL) return;",
        "  visit(r->data);",
        "  preorder(r->left);",
        "  preorder(r->right); }"
    };
    const vector<string> CODE_INORDER = {
        "void inorder(Node* r) {",
        "  if (r == NULL) return;",
        "  inorder(r->left);",
        "  visit(r->data);",
        "  inorder(r->right); }"
    };
    const vector<string> CODE_POSTORDER = {
        "void postorder(Node* r) {",
        "  if (r == NULL) return;",
        "  postorder(r->left);",
        "  postorder(r->right);",
        "  visit(r->data); }"
    };
    const vector<string> CODE_LEVEL = {
        "queue.push(root);",
        "while (!queue.empty()) {",
        "  Node* n = queue.pop();",
        "  visit(n->data);",
        "  push the children of n; }"
    };

    struct TreeView {
        int cur;       // node being looked at
        int found;
        int added;
        int removed;
        const vector<int>* done; // ids of nodes that are finished (visited by a traversal)
        long comparisons;

        TreeView() : cur(-1), found(-1), added(-1), removed(-1), done(nullptr), comparisons(0) {}
    };

    void collectNodes(BSTNode* node, const TreeView& view, vector<TreeNodeInfo>& out) {
        if (node == nullptr) {
            return;
        }
        TreeNodeInfo info;
        info.id = node->id;
        info.value = node->data;
        info.left = node->left != nullptr ? node->left->id : -1;
        info.right = node->right != nullptr ? node->right->id : -1;
        if (view.done != nullptr) {
            for (size_t i = 0; i < view.done->size(); i++) {
                if ((*view.done)[i] == node->id) info.role = ROLE_DONE;
            }
        }
        if (node->id == view.cur) info.role = ROLE_CURRENT;
        if (node->id == view.found) info.role = ROLE_FOUND;
        if (node->id == view.added) info.role = ROLE_NEW;
        if (node->id == view.removed) info.role = ROLE_REMOVED;
        out.push_back(info);
        collectNodes(node->left, view, out);
        collectNodes(node->right, view, out);
    }

    void recordTree(BSTNode* root, const string& message, int line, const TreeView& view) {
        if (!Recorder::enabled()) {
            return;
        }
        Frame f;
        f.kind = KIND_TREE;
        f.message = message;
        f.codeLine = line;
        f.comparisons = view.comparisons;
        f.rootId = root != nullptr ? root->id : -1;
        collectNodes(root, view, f.nodes);
        Recorder::add(f);
    }
}

// ---------- the tree itself ----------

BST::BST() {
    root = nullptr;
    count = 0;
    comparisons = 0;
    nextId = 1;
    quiet = false;
}

BST::~BST() {
    clear();
}

bool BST::isEmpty() const {
    return root == nullptr;
}

int BST::size() const {
    return count;
}

long BST::lastComparisons() const {
    return comparisons;
}

void BST::destroy(BSTNode* node) {
    if (node == nullptr) {
        return;
    }
    destroy(node->left);
    destroy(node->right);
    delete node;
}

void BST::clear() {
    destroy(root);
    root = nullptr;
    count = 0;
}

int BST::heightOf(BSTNode* node) const {
    if (node == nullptr) {
        return 0;
    }
    int leftHeight = heightOf(node->left);
    int rightHeight = heightOf(node->right);
    return 1 + (leftHeight > rightHeight ? leftHeight : rightHeight);
}

int BST::height() const {
    return heightOf(root);
}

bool BST::insert(int value) {
    Recorder::begin("Insert into the BST", CODE_INSERT);
    comparisons = 0;
    TreeView v;
    BSTNode* newNode = new BSTNode(value, nextId);

    if (root == nullptr) {
        root = newNode;
        nextId++;
        count++;
        Trace::say("tree was empty, " + to_string(value) + " becomes the root");
        v.added = newNode->id;
        recordTree(root, "The tree is empty (NULL), so " + to_string(value) + " becomes the root.", 1, v);
        return true;
    }

    BSTNode* current = root;
    while (true) {
        comparisons++;
        v.comparisons = comparisons;
        v.cur = current->id;
        if (value == current->data) {
            delete newNode; // duplicates are not allowed
            Trace::say(to_string(value) + " is already in the tree");
            v.cur = -1;
            v.found = current->id;
            recordTree(root, to_string(value) + " is already in the tree. Duplicates are not allowed, nothing is inserted.", 3, v);
            return false;
        }
        if (value < current->data) {
            Trace::say(to_string(value) + " < " + to_string(current->data) + ", go left");
            recordTree(root, to_string(value) + " < " + to_string(current->data) + ", so go to the left child.", 2, v);
            if (current->left == nullptr) {
                current->left = newNode;
                break;
            }
            current = current->left;
        }
        else {
            Trace::say(to_string(value) + " > " + to_string(current->data) + ", go right");
            recordTree(root, to_string(value) + " > " + to_string(current->data) + ", so go to the right child.", 3, v);
            if (current->right == nullptr) {
                current->right = newNode;
                break;
            }
            current = current->right;
        }
    }
    nextId++;
    count++;
    Trace::say("empty spot found, " + to_string(value) + " inserted");
    v.cur = -1;
    v.added = newNode->id;
    recordTree(root, "Reached an empty spot (NULL). The new node is inserted here: " + to_string(comparisons) + " comparisons, O(h).", 1, v);
    return true;
}

bool BST::search(int key) {
    Recorder::begin("Search in the BST", CODE_SEARCH);
    comparisons = 0;
    TreeView v;
    BSTNode* current = root;
    while (current != nullptr) {
        comparisons++;
        v.comparisons = comparisons;
        v.cur = current->id;
        if (key == current->data) {
            Trace::say("found " + to_string(key));
            v.cur = -1;
            v.found = current->id;
            recordTree(root, "Found " + to_string(key) + " after " + to_string(comparisons) + " comparison(s).", 2, v);
            return true;
        }
        if (key < current->data) {
            Trace::say(to_string(key) + " < " + to_string(current->data) + ", go left");
            recordTree(root, to_string(key) + " < " + to_string(current->data) + ", so search the left subtree.", 3, v);
            current = current->left;
        }
        else {
            Trace::say(to_string(key) + " > " + to_string(current->data) + ", go right");
            recordTree(root, to_string(key) + " > " + to_string(current->data) + ", so search the right subtree.", 4, v);
            current = current->right;
        }
    }
    Trace::say("reached NULL, " + to_string(key) + " is not in the tree");
    TreeView end;
    end.comparisons = comparisons;
    recordTree(root, "Reached NULL. " + to_string(key) + " is not in the tree.", 1, end);
    return false;
}

BSTNode* BST::removeFrom(BSTNode* node, int key, bool& removed) {
    if (node == nullptr) {
        return nullptr; // key not found
    }
    TreeView v;
    v.cur = node->id;
    if (key < node->data) {
        if (!quiet) recordTree(root, to_string(key) + " < " + to_string(node->data) + ", so look in the left subtree.", 2, v);
        node->left = removeFrom(node->left, key, removed);
        return node;
    }
    if (key > node->data) {
        if (!quiet) recordTree(root, to_string(key) + " > " + to_string(node->data) + ", so look in the right subtree.", 3, v);
        node->right = removeFrom(node->right, key, removed);
        return node;
    }

    // this is the node to delete
    removed = true;
    v.cur = -1;
    v.removed = node->id;
    if (node->left == nullptr && node->right == nullptr) {
        Trace::say(to_string(key) + " is a leaf, simply removed");
        if (!quiet) recordTree(root, to_string(key) + " is a leaf (no children). It is simply removed.", 5, v);
        delete node;
        return nullptr;
    }
    if (node->left == nullptr) {
        BSTNode* child = node->right;
        Trace::say(to_string(key) + " has only a right child, the child takes its place");
        if (!quiet) recordTree(root, to_string(key) + " has only a right child. That child takes its place.", 6, v);
        delete node;
        return child;
    }
    if (node->right == nullptr) {
        BSTNode* child = node->left;
        Trace::say(to_string(key) + " has only a left child, the child takes its place");
        if (!quiet) recordTree(root, to_string(key) + " has only a left child. That child takes its place.", 6, v);
        delete node;
        return child;
    }

    // two children: copy the smallest value of the right subtree here, then delete that node
    BSTNode* successor = node->right;
    while (successor->left != nullptr) {
        successor = successor->left;
    }
    Trace::say(to_string(key) + " has two children, replaced by its inorder successor " + to_string(successor->data));
    v.found = successor->id;
    if (!quiet) recordTree(root, to_string(key) + " has two children. Its inorder successor is " + to_string(successor->data) + " (the smallest value of the right subtree).", 8, v);
    node->data = successor->data;
    TreeView copied;
    copied.cur = node->id;
    copied.removed = successor->id;
    if (!quiet) recordTree(root, "Copy " + to_string(successor->data) + " into the node. The successor node is deleted next.", 9, copied);
    bool ignored = false;
    bool wasQuiet = quiet;
    quiet = true; // the second removal is only a helper step, it does not need its own frames
    node->right = removeFrom(node->right, successor->data, ignored);
    quiet = wasQuiet;
    return node;
}

bool BST::remove(int key) {
    Recorder::begin("Delete from the BST", CODE_REMOVE);
    bool removed = false;
    root = removeFrom(root, key, removed);
    if (removed) {
        count--;
        recordTree(root, to_string(key) + " has been deleted.", 11, TreeView());
    }
    else {
        recordTree(root, to_string(key) + " was not found, nothing is deleted.", 1, TreeView());
    }
    return removed;
}

bool BST::findMin(int& value) const {
    if (root == nullptr) {
        return false;
    }
    BSTNode* current = root;
    while (current->left != nullptr) {
        current = current->left;
    }
    value = current->data;
    return true;
}

bool BST::findMax(int& value) const {
    if (root == nullptr) {
        return false;
    }
    BSTNode* current = root;
    while (current->right != nullptr) {
        current = current->right;
    }
    value = current->data;
    return true;
}

void BST::preorderFrom(BSTNode* node, int out[], int& index, int maxCount) const {
    if (node == nullptr || index >= maxCount) {
        return;
    }
    out[index++] = node->data;
    TreeView v;
    v.cur = node->id;
    visitedTrail.push_back(node->id);
    v.done = &visitedTrail;
    recordTree(root, "Visit " + to_string(node->data) + " first, then its left and right subtrees.", 2, v);
    preorderFrom(node->left, out, index, maxCount);
    preorderFrom(node->right, out, index, maxCount);
}

void BST::inorderFrom(BSTNode* node, int out[], int& index, int maxCount) const {
    if (node == nullptr) {
        return;
    }
    TreeView v;
    v.cur = node->id;
    v.done = &visitedTrail;
    recordTree(root, "At " + to_string(node->data) + ": go through the left subtree first.", 2, v);
    inorderFrom(node->left, out, index, maxCount);
    if (index < maxCount) {
        out[index++] = node->data;
        visitedTrail.push_back(node->id);
        recordTree(root, "Visit " + to_string(node->data) + ".", 3, v);
    }
    inorderFrom(node->right, out, index, maxCount);
}

void BST::postorderFrom(BSTNode* node, int out[], int& index, int maxCount) const {
    if (node == nullptr) {
        return;
    }
    TreeView v;
    v.cur = node->id;
    v.done = &visitedTrail;
    recordTree(root, "At " + to_string(node->data) + ": go through both subtrees first.", 2, v);
    postorderFrom(node->left, out, index, maxCount);
    postorderFrom(node->right, out, index, maxCount);
    if (index < maxCount) {
        out[index++] = node->data;
        visitedTrail.push_back(node->id);
        recordTree(root, "Visit " + to_string(node->data) + " last.", 4, v);
    }
}

int BST::preorder(int out[], int maxCount) const {
    Recorder::begin("Preorder traversal", CODE_PREORDER);
    visitedTrail.clear();
    int index = 0;
    preorderFrom(root, out, index, maxCount);
    return index;
}

int BST::inorder(int out[], int maxCount) const {
    Recorder::begin("Inorder traversal", CODE_INORDER);
    visitedTrail.clear();
    int index = 0;
    inorderFrom(root, out, index, maxCount);
    return index;
}

int BST::postorder(int out[], int maxCount) const {
    Recorder::begin("Postorder traversal", CODE_POSTORDER);
    visitedTrail.clear();
    int index = 0;
    postorderFrom(root, out, index, maxCount);
    return index;
}

int BST::levelOrder(int out[], int maxCount) const {
    Recorder::begin("Level order traversal", CODE_LEVEL);
    visitedTrail.clear();
    if (root == nullptr) {
        return 0;
    }
    BSTNode** queue = new BSTNode*[count + 1];
    int head = 0;
    int tail = 0;
    int index = 0;
    queue[tail++] = root;
    while (head < tail && index < maxCount) {
        BSTNode* node = queue[head++];
        out[index++] = node->data;
        visitedTrail.push_back(node->id);
        TreeView v;
        v.cur = node->id;
        v.done = &visitedTrail;
        recordTree(root, "Visit " + to_string(node->data) + ", then queue its children.", 3, v);
        if (node->left != nullptr) queue[tail++] = node->left;
        if (node->right != nullptr) queue[tail++] = node->right;
    }
    delete[] queue;
    return index;
}

void BST::printSideways(BSTNode* node, int indent) const {
    if (node == nullptr) {
        return;
    }
    printSideways(node->right, indent + 6);
    cout << string(indent, ' ') << node->data << endl;
    printSideways(node->left, indent + 6);
}

void BST::displayTree() const {
    if (root == nullptr) {
        cout << "Tree is empty." << endl;
        return;
    }
    printSideways(root, 0);
}
