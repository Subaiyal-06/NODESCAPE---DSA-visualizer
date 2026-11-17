// General binary tree. New values are placed in the first free spot, level by level
// (so the tree stays as complete as possible). Search is a plain scan, not a BST search.

#ifndef BINARYTREE_H
#define BINARYTREE_H

class BTNode {
public:
    int data;
    BTNode* left;
    BTNode* right;

    BTNode(int data) {
        this->data = data;
        this->left = nullptr;
        this->right = nullptr;
    }
};

class BinaryTree {
private:
    BTNode* root;
    int count;

    void destroy(BTNode* node);
    int heightOf(BTNode* node) const;
    int leavesOf(BTNode* node) const;
    bool contains(BTNode* node, int key) const;
    void preorderFrom(BTNode* node, int out[], int& index, int maxCount) const;
    void inorderFrom(BTNode* node, int out[], int& index, int maxCount) const;
    void postorderFrom(BTNode* node, int out[], int& index, int maxCount) const;
    void printSideways(BTNode* node, int indent) const;

public:
    BinaryTree();
    ~BinaryTree();

    BinaryTree(const BinaryTree&) = delete;
    BinaryTree& operator=(const BinaryTree&) = delete;

    bool isEmpty() const;
    int size() const;
    int height() const;      // empty tree = 0, one node = 1
    int leafCount() const;

    void insertLevelOrder(int value);
    bool search(int key) const;

    // Traversals copy the values into out[] and return how many were written
    int preorder(int out[], int maxCount) const;
    int inorder(int out[], int maxCount) const;
    int postorder(int out[], int maxCount) const;
    int levelOrder(int out[], int maxCount) const;

    void clear();
    void displayTree() const; // the tree turned sideways: right side on top
};

#endif
