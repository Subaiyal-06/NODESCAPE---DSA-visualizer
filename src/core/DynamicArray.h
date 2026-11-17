// Dynamic array with linear and binary search. Indexes start from 0, like a normal array.

#ifndef DYNAMICARRAY_H
#define DYNAMICARRAY_H

#include <string>

class DynamicArray {
private:
    int* data;
    int capacity;
    int count;
    long comparisons; // comparisons made by the last search

    void grow();

public:
    DynamicArray(int initialCapacity = 4);
    ~DynamicArray();

    DynamicArray(const DynamicArray&) = delete;
    DynamicArray& operator=(const DynamicArray&) = delete;

    int size() const;
    int getCapacity() const;
    bool isEmpty() const;

    void append(int value);
    bool insertAt(int index, int value);  // index from 0 to size
    bool removeAt(int index);             // index from 0 to size - 1
    bool getAt(int index, int& value) const;

    int linearSearch(int key);            // index of the first match, or -1
    int binarySearch(int key);            // index, -1 if missing, -2 if the array is not sorted
    bool isSorted() const;
    void sortAscending();                 // insertion sort
    long lastComparisons() const;

    void clear();
    void display() const;
    void showArray(const std::string& message) const; // records one calm picture of the array (for the graphical app)
};

#endif
