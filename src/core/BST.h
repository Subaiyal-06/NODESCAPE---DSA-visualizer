// Binary Search Tree: smaller values go left, larger values go right. No duplicates.

#ifndef BST_H
#define BST_H

#include <vector>

class BSTNode {
public:
    int id;     // never changes, lets the recorder follow a node from one step to the next
    int data;
    BSTNode* left;
    BSTNode* right;

    BSTNode(int data, int id) {
        this->id = id;
        this->data = data;
        this->left = nullptr;
        this->right = nullptr;
    }
};

class BST {
private:
    BSTNode* root;
    int count;
    long comparisons; // comparisons made by the last insert or search
    int nextId;       // id for the next node that is created
    bool quiet;       // true while a helper step must not record frames
    mutable std::vector<int> visitedTrail; // ids visited so far by a traversal (for the recorder)

    void destroy(BSTNode* node);
    int heightOf(BSTNode* node) const;
    BSTNode* removeFrom(BSTNode* node, int key, bool& removed);
    void preorderFrom(BSTNode* node, int out[], int& index, int maxCount) const;
    void inorderFrom(BSTNode* node, int out[], int& index, int maxCount) const;
    void postorderFrom(BSTNode* node, int out[], int& index, int maxCount) const;
    void printSideways(BSTNode* node, int indent) const;

public:
    BST();
    ~BST();

    BST(const BST&) = delete;
    BST& operator=(const BST&) = delete;

    bool isEmpty() const;
    int size() const;
    int height() const;
    long lastComparisons() const;

    bool insert(int value);   // false if the value is already in the tree
    bool search(int key);
    bool remove(int key);     // false if the value is not in the tree
    bool findMin(int& value) const;
    bool findMax(int& value) const;

    int preorder(int out[], int maxCount) const;
    int inorder(int out[], int maxCount) const;
    int postorder(int out[], int maxCount) const;
    int levelOrder(int out[], int maxCount) const;

    void clear();
    void displayTree() const;
};

#endif
