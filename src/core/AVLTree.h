// AVL Tree: a BST that keeps itself balanced. For every node the heights of the
// left and right subtree differ by at most 1. Rotations fix any imbalance.

#ifndef AVLTREE_H
#define AVLTREE_H

class AVLNode {
public:
    int data;
    AVLNode* left;
    AVLNode* right;
    int height; // height of the subtree rooted here, a leaf has height 1
    int id;     // never changes, lets the recorder follow a node while it moves in rotations

    AVLNode(int data, int id = 0) {
        this->id = id;
        this->data = data;
        this->left = nullptr;
        this->right = nullptr;
        this->height = 1;
    }
};

class AVLTree {
private:
    AVLNode* root;
    int count;
    int rotations; // total rotations done so far
    int nextId;    // id for the next node that is created

    void destroy(AVLNode* node);
    int heightOf(AVLNode* node) const;
    int balanceFactor(AVLNode* node) const;
    void updateHeight(AVLNode* node);
    AVLNode* rotateRight(AVLNode* y);
    AVLNode* rotateLeft(AVLNode* x);
    AVLNode* rebalance(AVLNode* node);
    AVLNode* insertInto(AVLNode* node, int value, bool& inserted);
    AVLNode* removeFrom(AVLNode* node, int key, bool& removed);
    bool checkNode(AVLNode* node, long long low, long long high, int& height) const;
    void inorderFrom(AVLNode* node, int out[], int& index, int maxCount) const;
    void preorderFrom(AVLNode* node, int out[], int& index, int maxCount) const;
    void printSideways(AVLNode* node, int indent) const;

public:
    AVLTree();
    ~AVLTree();

    AVLTree(const AVLTree&) = delete;
    AVLTree& operator=(const AVLTree&) = delete;

    bool isEmpty() const;
    int size() const;
    int height() const;
    int rotationCount() const;

    bool insert(int value);   // false if the value is already in the tree
    bool remove(int key);     // false if the value is not in the tree
    bool search(int key) const;

    int inorder(int out[], int maxCount) const;
    int preorder(int out[], int maxCount) const;

    // Used by the tests: checks stored heights, balance factors and BST order of the whole tree
    bool isValidAVL() const;

    void clear();
    void displayTree() const; // sideways, each value followed by its balance factor
};

#endif
