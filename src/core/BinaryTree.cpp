#include "BinaryTree.h"
#include "Trace.h"
#include <iostream>
#include <string>
using namespace std;

BinaryTree::BinaryTree() {
    root = nullptr;
    count = 0;
}

BinaryTree::~BinaryTree() {
    clear();
}

bool BinaryTree::isEmpty() const {
    return root == nullptr;
}

int BinaryTree::size() const {
    return count;
}

void BinaryTree::destroy(BTNode* node) {
    if (node == nullptr) {
        return;
    }
    destroy(node->left);
    destroy(node->right);
    delete node; // children first, then the node itself (postorder)
}

void BinaryTree::clear() {
    destroy(root);
    root = nullptr;
    count = 0;
}

int BinaryTree::heightOf(BTNode* node) const {
    if (node == nullptr) {
        return 0;
    }
    int leftHeight = heightOf(node->left);
    int rightHeight = heightOf(node->right);
    return 1 + (leftHeight > rightHeight ? leftHeight : rightHeight);
}

int BinaryTree::height() const {
    return heightOf(root);
}

int BinaryTree::leavesOf(BTNode* node) const {
    if (node == nullptr) {
        return 0;
    }
    if (node->left == nullptr && node->right == nullptr) {
        return 1;
    }
    return leavesOf(node->left) + leavesOf(node->right);
}

int BinaryTree::leafCount() const {
    return leavesOf(root);
}

void BinaryTree::insertLevelOrder(int value) {
    BTNode* newNode = new BTNode(value);
    count++;

    if (root == nullptr) {
        root = newNode;
        Trace::say("tree was empty, " + to_string(value) + " becomes the root");
        return;
    }

    // a queue of nodes to visit, level by level
    BTNode** queue = new BTNode*[count + 1];
    int head = 0;
    int tail = 0;
    queue[tail++] = root;

    while (head < tail) {
        BTNode* node = queue[head++];
        Trace::say("checking node " + to_string(node->data));
        if (node->left == nullptr) {
            node->left = newNode;
            Trace::say(to_string(value) + " placed as left child of " + to_string(node->data));
            break;
        }
        queue[tail++] = node->left;
        if (node->right == nullptr) {
            node->right = newNode;
            Trace::say(to_string(value) + " placed as right child of " + to_string(node->data));
            break;
        }
        queue[tail++] = node->right;
    }
    delete[] queue;
}

bool BinaryTree::contains(BTNode* node, int key) const {
    if (node == nullptr) {
        return false;
    }
    if (node->data == key) {
        return true;
    }
    return contains(node->left, key) || contains(node->right, key);
}

bool BinaryTree::search(int key) const {
    return contains(root, key);
}

void BinaryTree::preorderFrom(BTNode* node, int out[], int& index, int maxCount) const {
    if (node == nullptr || index >= maxCount) {
        return;
    }
    out[index++] = node->data; // root, then left, then right
    Trace::say("visit " + to_string(node->data));
    preorderFrom(node->left, out, index, maxCount);
    preorderFrom(node->right, out, index, maxCount);
}

void BinaryTree::inorderFrom(BTNode* node, int out[], int& index, int maxCount) const {
    if (node == nullptr) {
        return;
    }
    inorderFrom(node->left, out, index, maxCount);
    if (index < maxCount) {
        out[index++] = node->data; // left, then root, then right
        Trace::say("visit " + to_string(node->data));
    }
    inorderFrom(node->right, out, index, maxCount);
}

void BinaryTree::postorderFrom(BTNode* node, int out[], int& index, int maxCount) const {
    if (node == nullptr) {
        return;
    }
    postorderFrom(node->left, out, index, maxCount);
    postorderFrom(node->right, out, index, maxCount);
    if (index < maxCount) {
        out[index++] = node->data; // left, then right, then root
        Trace::say("visit " + to_string(node->data));
    }
}

int BinaryTree::preorder(int out[], int maxCount) const {
    int index = 0;
    preorderFrom(root, out, index, maxCount);
    return index;
}

int BinaryTree::inorder(int out[], int maxCount) const {
    int index = 0;
    inorderFrom(root, out, index, maxCount);
    return index;
}

int BinaryTree::postorder(int out[], int maxCount) const {
    int index = 0;
    postorderFrom(root, out, index, maxCount);
    return index;
}

int BinaryTree::levelOrder(int out[], int maxCount) const {
    if (root == nullptr) {
        return 0;
    }
    BTNode** queue = new BTNode*[count + 1];
    int head = 0;
    int tail = 0;
    int index = 0;
    queue[tail++] = root;
    while (head < tail && index < maxCount) {
        BTNode* node = queue[head++];
        out[index++] = node->data;
        Trace::say("visit " + to_string(node->data));
        if (node->left != nullptr) queue[tail++] = node->left;
        if (node->right != nullptr) queue[tail++] = node->right;
    }
    delete[] queue;
    return index;
}

void BinaryTree::printSideways(BTNode* node, int indent) const {
    if (node == nullptr) {
        return;
    }
    printSideways(node->right, indent + 6);
    cout << string(indent, ' ') << node->data << endl;
    printSideways(node->left, indent + 6);
}

void BinaryTree::displayTree() const {
    if (root == nullptr) {
        cout << "Tree is empty." << endl;
        return;
    }
    printSideways(root, 0);
}
