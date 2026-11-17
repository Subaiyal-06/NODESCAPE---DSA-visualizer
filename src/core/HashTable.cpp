#include "HashTable.h"
#include "Trace.h"
#include "Recorder.h"
#include <vector>
#include <iostream>
#include <string>
using namespace std;

// ---------- recording helpers (used only when the recorder is switched on) ----------

namespace {
    bool gQuiet = false; // true while insert or remove calls search for its own purposes

    const vector<string> CODE_INSERT_CHAIN = {
        "index = hash(key) % size;",
        "if (bucket[index] == NULL) bucket[index] = new node;",
        "else walk to the end of the chain and append the new node;"
    };
    const vector<string> CODE_INSERT_OPEN = {
        "home = hash(key) % size;",
        "for (attempt = 0; attempt < size; attempt++) {",
        "  slot = probe(home, attempt);   // linear +i, quadratic +i*i, double +i*h2",
        "  if (slot is free) { table[slot] = key; return true; }",
        "  // otherwise: collision, try the next slot }"
    };
    const vector<string> CODE_SEARCH = {
        "index = hash(key) % size;",
        "walk the chain, or probe slot after slot,",
        "until the key is found or an empty slot ends the search;",
        "return found ? index : -1;"
    };
    const vector<string> CODE_REMOVE = {
        "index = search(key);",
        "if (chaining) unlink the node from its chain;",
        "else mark the slot as deleted, never as empty,",
        "  or later searches would stop too early;"
    };

    const char* methodName(CollisionMethod m) {
        if (m == CHAINING) return "chaining";
        if (m == LINEAR_PROBING) return "linear probing";
        if (m == QUADRATIC_PROBING) return "quadratic probing";
        return "double hashing";
    }

    const char* functionName(HashFunction f) {
        if (f == FOLDING) return "folding";
        if (f == MID_SQUARE) return "mid-square";
        return "division";
    }
}

void HashTable::snap(const string& message, int line, int row, int rowRole, int chainNode, int nodeRole, int key) const {
    if (!Recorder::enabled() || gQuiet) return;
    Frame f;
    f.kind = KIND_HASH;
    f.message = message;
    f.codeLine = line;
    f.comparisons = probes;
    f.auxLabel = string(methodName(method)) + ", " + functionName(function) + " hash, table size " + to_string(tableSize);
    if (key >= 0) {
        f.info.push_back("hash(" + to_string(key) + ") = " + to_string(hashOf(key)) + "   (" + functionName(function) + " method, " + to_string(tableSize) + " slots)");
    }
    for (int i = 0; i < tableSize; i++) {
        Role rr = (i == row) ? (Role)rowRole : ROLE_NORMAL;
        if (method == CHAINING) {
            f.cells.push_back(Cell::Empty(rr));
            vector<Cell> chain;
            int pos = 0;
            for (HNode* t = buckets[i]; t != nullptr; t = t->next, pos++) {
                Role r = ROLE_NORMAL;
                if (i == row && pos == chainNode) r = (Role)nodeRole;
                chain.push_back(Cell(t->key, r));
            }
            f.chains.push_back(chain);
        }
        else if (state[i] == 'O') {
            f.cells.push_back(Cell(keys[i], rr));
        }
        else {
            f.cells.push_back(Cell::Empty(rr));
            if (state[i] == 'D') f.markers.push_back(Marker(i, "deleted"));
        }
    }
    Recorder::add(f);
}

HashTable::HashTable(int size, CollisionMethod collisionMethod, HashFunction hashFunction) {
    if (size < 2) size = 2;
    if (size > MAXSIZE) size = MAXSIZE;
    tableSize = size;
    method = collisionMethod;
    function = hashFunction;
    items = 0;
    probes = 0;
    collisions = 0;
    for (int i = 0; i < MAXSIZE; i++) {
        buckets[i] = nullptr;
        keys[i] = 0;
        state[i] = 'E';
    }
}

HashTable::~HashTable() {
    clear();
}

int HashTable::getTableSize() const {
    return tableSize;
}

int HashTable::itemCount() const {
    return items;
}

int HashTable::lastProbes() const {
    return probes;
}

int HashTable::lastCollisions() const {
    return collisions;
}

double HashTable::loadFactor() const {
    return (double)items / tableSize;
}

CollisionMethod HashTable::getMethod() const {
    return method;
}

HashFunction HashTable::getFunction() const {
    return function;
}

int HashTable::hashOf(int key) const {
    if (function == FOLDING) {
        int sum = 0;
        while (key > 0) {
            sum += key % 100; // take two digits at a time from the right
            key /= 100;
        }
        return sum % tableSize;
    }
    if (function == MID_SQUARE) {
        long long square = (long long)key * key;
        if (square < 100) {
            return (int)(square % tableSize);
        }
        int digits = 0;
        long long copy = square;
        while (copy > 0) {
            digits++;
            copy /= 10;
        }
        long long divisor = 1;
        for (int i = 0; i < (digits - 2) / 2; i++) {
            divisor *= 10;
        }
        int middle = (int)((square / divisor) % 100);
        return middle % tableSize;
    }
    return key % tableSize;
}

int HashTable::hashValue(int key) const {
    return hashOf(key);
}

// the slot to try on the given attempt (attempt 0 is the home slot)
int HashTable::slotFor(int key, int attempt) const {
    int home = hashOf(key);
    if (method == LINEAR_PROBING) {
        return (home + attempt) % tableSize;
    }
    if (method == QUADRATIC_PROBING) {
        return (home + attempt * attempt) % tableSize;
    }
    int step = 1 + (key % (tableSize - 1)); // second hash function, never 0
    return (home + attempt * step) % tableSize;
}

bool HashTable::insert(int key) {
    probes = 0;
    collisions = 0;
    if (!gQuiet) Recorder::begin("Insert into the hash table", method == CHAINING ? CODE_INSERT_CHAIN : CODE_INSERT_OPEN);
    if (key < 0) {
        snap("Keys must be 0 or more in this table.", 0, -1, 0, -1, 0);
        return false;
    }
    bool before = gQuiet;
    gQuiet = true;
    int existing = search(key);
    gQuiet = before;
    if (existing != -1) {
        Trace::say(to_string(key) + " is already in the table");
        snap(to_string(key) + " is already in the table (slot " + to_string(existing) + "). Duplicates are not stored.", 0, existing, ROLE_FOUND, -1, ROLE_FOUND, key);
        return false;
    }
    probes = 0;
    int home = hashOf(key);
    if (method == CHAINING) {
        int index = home;
        Trace::say("hash(" + to_string(key) + ") = " + to_string(index));
        snap("The hash function sends " + to_string(key) + " to slot " + to_string(index) + ".", 0, index, ROLE_CURRENT, -1, ROLE_NORMAL, key);
        HNode* newNode = new HNode(key);
        if (buckets[index] == nullptr) {
            snap("Slot " + to_string(index) + " is empty, so the new node becomes its chain.", 1, index, ROLE_CURRENT, -1, ROLE_NORMAL, key);
            buckets[index] = newNode;
        }
        else {
            HNode* temp = buckets[index];
            collisions++;
            int pos = 0;
            snap("Collision: slot " + to_string(index) + " already holds " + to_string(temp->key) + ". The new key joins the chain.", 2, index, ROLE_KEPT, pos, ROLE_KEPT, key);
            while (temp->next != nullptr) {
                temp = temp->next;
                collisions++;
                pos++;
                snap("Walk along the chain to its end (" + to_string(temp->key) + ").", 2, index, ROLE_KEPT, pos, ROLE_CURRENT, key);
            }
            temp->next = newNode; // collision: the key joins the chain of that slot
            Trace::say("slot " + to_string(index) + " already has " + to_string(collisions) + " key(s), added at the end of the chain");
        }
        probes = collisions + 1;
        items++;
        int last = 0;
        for (HNode* t = buckets[index]; t != nullptr && t->next != nullptr; t = t->next) last++;
        snap(to_string(key) + " stored in slot " + to_string(index) + " (" + to_string(collisions) + " collision step(s)).", 2, index, ROLE_NEW, last, ROLE_NEW, key);
        return true;
    }
    for (int attempt = 0; attempt < tableSize; attempt++) {
        int index = slotFor(key, attempt);
        probes++;
        string how = attempt == 0 ? "Home slot of " + to_string(key) + " is " + to_string(index) + "." : "Probe attempt " + to_string(attempt) + ": slot " + to_string(index) + ".";
        snap(how, attempt == 0 ? 0 : 2, index, ROLE_CURRENT, -1, ROLE_NORMAL, key);
        if (state[index] != 'O') { // empty or deleted slots can be reused
            keys[index] = key;
            state[index] = 'O';
            items++;
            Trace::say("slot " + to_string(index) + " is free, " + to_string(key) + " stored");
            snap("Slot " + to_string(index) + " is free. " + to_string(key) + " is stored here (" + to_string(collisions) + " collision(s)).", 3, index, ROLE_NEW, -1, ROLE_NEW, key);
            return true;
        }
        collisions++;
        Trace::say("slot " + to_string(index) + " is taken by " + to_string(keys[index]) + ", collision");
        snap("Collision: slot " + to_string(index) + " is taken by " + to_string(keys[index]) + ". Try the next probe.", 4, index, ROLE_KEPT, -1, ROLE_NORMAL, key);
    }
    snap("No free slot could be reached. The table is full for this probe sequence.", 4, -1, 0, -1, 0, key);
    return false; // no free slot could be reached
}

int HashTable::search(int key) {
    probes = 0;
    if (!gQuiet) Recorder::begin("Search in the hash table", CODE_SEARCH);
    if (key < 0) {
        snap("Keys are 0 or more in this table.", 0, -1, 0, -1, 0);
        return -1;
    }
    if (method == CHAINING) {
        int index = hashOf(key);
        snap("The hash function sends " + to_string(key) + " to slot " + to_string(index) + ".", 0, index, ROLE_CURRENT, -1, ROLE_NORMAL, key);
        int pos = 0;
        for (HNode* temp = buckets[index]; temp != nullptr; temp = temp->next, pos++) {
            probes++;
            if (temp->key == key) {
                snap("Found " + to_string(key) + " after " + to_string(probes) + " step(s).", 2, index, ROLE_FOUND, pos, ROLE_FOUND, key);
                return index;
            }
            snap(to_string(temp->key) + " is not " + to_string(key) + ". Next node of the chain.", 1, index, ROLE_CURRENT, pos, ROLE_CURRENT, key);
        }
        snap("End of the chain. " + to_string(key) + " is not in the table.", 2, index, ROLE_KEPT, -1, ROLE_NORMAL, key);
        return -1;
    }
    for (int attempt = 0; attempt < tableSize; attempt++) {
        int index = slotFor(key, attempt);
        probes++;
        if (state[index] == 'E') {
            snap("Slot " + to_string(index) + " is empty. The key would have been placed here or before, so it is not in the table.", 2, index, ROLE_KEPT, -1, ROLE_NORMAL, key);
            return -1; // an empty slot ends the search: the key would have been placed here or before
        }
        if (state[index] == 'O' && keys[index] == key) {
            snap("Found " + to_string(key) + " in slot " + to_string(index) + " after " + to_string(probes) + " probe(s).", 2, index, ROLE_FOUND, -1, ROLE_NORMAL, key);
            return index;
        }
        snap(state[index] == 'D' ? "Slot " + to_string(index) + " was deleted. Keep probing, the key may be further on." : "Slot " + to_string(index) + " holds " + to_string(keys[index]) + ", not " + to_string(key) + ". Probe again.", 1, index, ROLE_CURRENT, -1, ROLE_NORMAL, key);
    }
    snap("Every slot was probed. " + to_string(key) + " is not in the table.", 2, -1, 0, -1, 0, key);
    return -1;
}

bool HashTable::remove(int key) {
    Recorder::begin("Delete from the hash table", CODE_REMOVE);
    bool before = gQuiet;
    gQuiet = true;
    int index = search(key);
    gQuiet = before;
    if (index == -1) {
        snap(to_string(key) + " is not in the table, nothing to delete.", 0, -1, 0, -1, 0, key);
        return false;
    }
    if (method == CHAINING) {
        int pos = 0;
        HNode* prev = nullptr;
        HNode* temp = buckets[index];
        while (temp->key != key) {
            prev = temp;
            temp = temp->next;
            pos++;
        }
        snap("Found " + to_string(key) + " in the chain of slot " + to_string(index) + ". Unlink it.", 1, index, ROLE_CURRENT, pos, ROLE_REMOVED, key);
        if (prev == nullptr) {
            buckets[index] = temp->next;
        }
        else {
            prev->next = temp->next;
        }
        delete temp;
        items--;
        snap(to_string(key) + " removed.", 1, index, ROLE_CURRENT, -1, ROLE_NORMAL, key);
        return true;
    }
    snap("Found " + to_string(key) + " in slot " + to_string(index) + ".", 0, index, ROLE_REMOVED, -1, ROLE_NORMAL, key);
    state[index] = 'D'; // mark as deleted, do not empty it, or searches would stop too early
    Trace::say("slot " + to_string(index) + " marked as deleted");
    items--;
    snap("The slot is marked deleted, not empty. An empty slot would make later searches stop too early.", 2, index, ROLE_NORMAL, -1, ROLE_NORMAL, key);
    return true;
}

void HashTable::clear() {
    for (int i = 0; i < MAXSIZE; i++) {
        while (buckets[i] != nullptr) {
            HNode* temp = buckets[i];
            buckets[i] = buckets[i]->next;
            delete temp;
        }
        keys[i] = 0;
        state[i] = 'E';
    }
    items = 0;
}

void HashTable::showTable(const string& message) const {
    Recorder::begin("Hash table", vector<string>());
    snap(message, -1, -1, 0, -1, 0);
}

void HashTable::display() const {
    for (int i = 0; i < tableSize; i++) {
        cout << "[" << i << "] ";
        if (method == CHAINING) {
            if (buckets[i] == nullptr) {
                cout << "-";
            }
            else {
                for (HNode* temp = buckets[i]; temp != nullptr; temp = temp->next) {
                    cout << temp->key << " -> ";
                }
                cout << "NULL";
            }
        }
        else if (state[i] == 'O') {
            cout << keys[i];
        }
        else if (state[i] == 'D') {
            cout << "(deleted)";
        }
        else {
            cout << "-";
        }
        cout << endl;
    }
}
