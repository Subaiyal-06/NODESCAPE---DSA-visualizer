// Tests for the hash table: hash functions and the four collision methods

#include "TestUtils.h"
#include "../src/core/HashTable.h"

int main() {
    // hash functions
    {
        HashTable division(10, CHAINING, DIVISION);
        check(division.hashValue(47) == 7 && division.hashValue(100) == 0, "division method");
        HashTable folding(10, CHAINING, FOLDING);
        check(folding.hashValue(123456) == 2, "folding method (56 + 34 + 12 = 102)");
        HashTable midSquare(10, CHAINING, MID_SQUARE);
        check(midSquare.hashValue(123) == 2 && midSquare.hashValue(4) == 6, "mid-square method");
    }

    // chaining
    {
        HashTable t(7, CHAINING, DIVISION);
        check(t.insert(7) && t.lastCollisions() == 0, "first key in a slot");
        check(t.insert(14) && t.lastCollisions() == 1, "second key in the same slot is a collision");
        check(t.insert(21) && t.lastCollisions() == 2, "third key joins the chain");
        check(t.insert(3), "key in another slot");
        check(!t.insert(14), "duplicate key");
        check(!t.insert(-5), "negative key");
        check(t.itemCount() == 4, "item count");
        check(t.search(21) == 0 && t.lastProbes() == 3, "search follows the chain");
        check(t.search(8) == -1, "missing key");
        check(t.remove(14) && t.search(14) == -1 && t.search(21) == 0, "remove from the middle of a chain");
        check(t.remove(7) && t.search(21) == 0, "remove the head of a chain");
        check(!t.remove(99), "remove a missing key");
        check(t.itemCount() == 2, "item count after removals");
        t.clear();
        check(t.itemCount() == 0 && t.search(3) == -1, "clear");
    }

    // linear probing
    {
        HashTable t(10, LINEAR_PROBING, DIVISION);
        check(t.insert(10) && t.insert(20) && t.insert(30), "three keys with the same home slot");
        check(t.search(10) == 0 && t.search(20) == 1 && t.search(30) == 2, "they sit in neighbouring slots");
        check(t.lastProbes() == 3, "probes counted");
        check(t.remove(20), "remove the middle key");
        check(t.search(30) == 2, "a deleted slot does not break the search");
        check(t.insert(40) && t.search(40) == 1, "a deleted slot is reused");
        check(!t.insert(30), "duplicate key");
        check(t.search(99) == -1, "missing key stops at an empty slot");

        HashTable full(5, LINEAR_PROBING, DIVISION);
        for (int k = 0; k < 5; k++) check(full.insert(k * 5), "fill the table");
        check(!full.insert(100), "full table rejects a new key");
        check(full.loadFactor() == 1.0, "load factor of a full table");
    }

    // quadratic probing
    {
        HashTable t(7, QUADRATIC_PROBING, DIVISION);
        check(t.insert(7) && t.insert(14) && t.insert(21), "insert keys with the same home slot");
        check(t.search(7) == 0 && t.search(14) == 1 && t.search(21) == 4, "slots home, home+1, home+4");
    }

    // double hashing
    {
        HashTable t(7, DOUBLE_HASHING, DIVISION);
        check(t.insert(7) && t.insert(14), "insert keys with the same home slot");
        check(t.search(7) == 0 && t.search(14) == 3, "the second hash gives the jump (1 + 14 % 6 = 3)");
        check(t.remove(7) && t.search(14) == 3, "remove and search again");
    }

    return finishTests();
}
