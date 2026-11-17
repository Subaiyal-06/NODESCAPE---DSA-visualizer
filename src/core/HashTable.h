// Hash table for non-negative int keys. The collision method and the hash function can be chosen.
// Open addressing (linear, quadratic, double hashing) stores keys in the table itself,
// chaining keeps a linked list in every slot.

#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <string>

enum CollisionMethod {
    CHAINING,
    LINEAR_PROBING,
    QUADRATIC_PROBING,
    DOUBLE_HASHING
};

enum HashFunction {
    DIVISION,   // key % size
    FOLDING,    // add the key in groups of two digits, then % size
    MID_SQUARE  // square the key, take the two middle digits, then % size
};

class HNode {
public:
    int key;
    HNode* next;

    HNode(int key) {
        this->key = key;
        this->next = nullptr;
    }
};

class HashTable {
public:
    static const int MAXSIZE = 31;

private:
    int tableSize;
    CollisionMethod method;
    HashFunction function;
    int items;
    int probes;      // slots looked at by the last insert or search
    int collisions;  // collisions met by the last insert

    HNode* buckets[MAXSIZE]; // used by chaining
    int keys[MAXSIZE];       // used by open addressing
    char state[MAXSIZE];     // 'E' empty, 'O' occupied, 'D' deleted (open addressing)

    int hashOf(int key) const;
    int slotFor(int key, int attempt) const;
    // recording: one picture of the table (row = slot, chainNode = position inside a chain, -1 for none)
    void snap(const std::string& message, int line, int row, int rowRole, int chainNode, int nodeRole, int key = -1) const;

public:
    HashTable(int size, CollisionMethod collisionMethod, HashFunction hashFunction);
    ~HashTable();

    HashTable(const HashTable&) = delete;
    HashTable& operator=(const HashTable&) = delete;

    int getTableSize() const;
    int itemCount() const;
    int lastProbes() const;
    int lastCollisions() const;
    double loadFactor() const;
    CollisionMethod getMethod() const;
    HashFunction getFunction() const;
    int hashValue(int key) const;   // the home slot of a key

    bool insert(int key);           // false for negative keys, duplicates, or when no free slot is found
    int search(int key);            // slot number (-1 if missing). With chaining it is the bucket number.
    bool remove(int key);

    void clear();
    void display() const;
    void showTable(const std::string& message) const; // records one calm picture of the table (for the graphical app)
};

#endif
