#include "DynamicArray.h"
#include "Trace.h"
#include "Recorder.h"
#include <iostream>
#include <string>
#include <vector>
using namespace std;

// ---------- recording helpers (used only when the recorder is switched on) ----------

namespace {
    const vector<string> CODE_APPEND = {
        "if (count == capacity) grow();      // allocate 2x and copy",
        "data[count] = value;",
        "count++;"
    };
    const vector<string> CODE_INSERT = {
        "if (count == capacity) grow();",
        "for (i = count; i > index; i--)",
        "  data[i] = data[i - 1];            // shift right",
        "data[index] = value;  count++;"
    };
    const vector<string> CODE_REMOVE = {
        "if (index < 0 || index >= count) return false;",
        "for (i = index; i < count - 1; i++)",
        "  data[i] = data[i + 1];            // shift left",
        "count--;"
    };
    const vector<string> CODE_LINEAR = {
        "for (i = 0; i < count; i++) {",
        "  if (data[i] == key) return i;",
        "}",
        "return -1;"
    };
    const vector<string> CODE_BINARY = {
        "low = 0;  high = count - 1;",
        "while (low <= high) {",
        "  mid = low + (high - low) / 2;",
        "  if (data[mid] == key) return mid;",
        "  if (data[mid] < key) low = mid + 1;   // keep the right half",
        "  else high = mid - 1;                  // keep the left half",
        "}  return -1;"
    };
    const vector<string> CODE_SORT = {
        "for (i = 1; i < count; i++) {",
        "  key = data[i];  j = i - 1;",
        "  while (j >= 0 && data[j] > key) { data[j + 1] = data[j]; j--; }",
        "  data[j + 1] = key;",
        "}"
    };

    // roles: one per slot (only the first `count` slots can hold values)
    void snap(const int* data, int capacity, int count, const vector<Role>& roles, const string& message, int line,
              const vector<Marker>& markers, long comparisons = 0) {
        if (!Recorder::enabled()) return;
        Frame f;
        f.kind = KIND_SLOTS;
        f.message = message;
        f.codeLine = line;
        f.comparisons = comparisons;
        for (int i = 0; i < capacity; i++) {
            Role r = i < (int)roles.size() ? roles[i] : ROLE_NORMAL;
            if (i < count) f.cells.push_back(Cell(data[i], r));
            else f.cells.push_back(Cell::Empty(r));
        }
        f.markers = markers;
        f.auxLabel = "size " + to_string(count) + " of capacity " + to_string(capacity);
        Recorder::add(f);
    }

    vector<Role> plain(int n) {
        return vector<Role>(n > 0 ? n : 0, ROLE_NORMAL);
    }
}

DynamicArray::DynamicArray(int initialCapacity) {
    if (initialCapacity < 1) {
        initialCapacity = 1;
    }
    capacity = initialCapacity;
    data = new int[capacity];
    count = 0;
    comparisons = 0;
}

DynamicArray::~DynamicArray() {
    delete[] data;
}

int DynamicArray::size() const {
    return count;
}

int DynamicArray::getCapacity() const {
    return capacity;
}

bool DynamicArray::isEmpty() const {
    return count == 0;
}

void DynamicArray::grow() {
    if (Recorder::enabled()) {
        vector<Role> r = plain(capacity);
        for (int i = 0; i < count; i++) r[i] = ROLE_KEPT;
        snap(data, capacity, count, r, "The array is full (" + to_string(count) + " of " + to_string(capacity) + "). It must grow: allocate a new array twice as big.", 0, vector<Marker>());
    }
    int* bigger = new int[capacity * 2];
    for (int i = 0; i < count; i++) {
        bigger[i] = data[i]; // copy everything into the bigger array
    }
    delete[] data;
    data = bigger;
    capacity = capacity * 2;
    Trace::say("array was full, capacity doubled to " + to_string(capacity));
    if (Recorder::enabled()) {
        vector<Role> r = plain(capacity);
        for (int i = 0; i < count; i++) r[i] = ROLE_NEW;
        snap(data, capacity, count, r, "Every element was copied into the new array. Capacity is now " + to_string(capacity) + ". The copy costs O(n), but it happens rarely.", 0, vector<Marker>());
    }
}

void DynamicArray::append(int value) {
    Recorder::begin("Append (dynamic array)", CODE_APPEND);
    if (count == capacity) {
        grow();
    }
    else {
        snap(data, capacity, count, plain(capacity), "There is room: " + to_string(count) + " of " + to_string(capacity) + " slots are used.", 0, vector<Marker>());
    }
    data[count] = value;
    count++;
    vector<Role> r = plain(capacity);
    r[count - 1] = ROLE_NEW;
    snap(data, capacity, count, r, "Store " + to_string(value) + " in slot " + to_string(count - 1) + " and increase count. O(1).", 2, vector<Marker>());
}

bool DynamicArray::insertAt(int index, int value) {
    Recorder::begin("Insert at index (dynamic array)", CODE_INSERT);
    if (index < 0 || index > count) {
        snap(data, capacity, count, plain(capacity), "Index " + to_string(index) + " does not exist (valid: 0 to " + to_string(count) + ").", 0, vector<Marker>());
        return false;
    }
    if (count == capacity) {
        grow();
    }
    for (int i = count; i > index; i--) {
        vector<Role> r = plain(capacity);
        r[i - 1] = ROLE_CURRENT;
        snap(data, capacity, count, r, "Shift " + to_string(data[i - 1]) + " from slot " + to_string(i - 1) + " one place to the right.", 2, vector<Marker>());
        data[i] = data[i - 1]; // shift right to make room
        vector<Role> r2 = plain(capacity);
        r2[i] = ROLE_NEW;
        snap(data, capacity, count + 1, r2, "Moved to slot " + to_string(i) + ".", 2, vector<Marker>());
    }
    Trace::say("shifted " + to_string(count - index) + " element(s) to the right");
    data[index] = value;
    count++;
    vector<Role> r = plain(capacity);
    r[index] = ROLE_NEW;
    snap(data, capacity, count, r, "Store " + to_string(value) + " in the gap at index " + to_string(index) + ". Shifting made this O(n).", 3, vector<Marker>());
    return true;
}

bool DynamicArray::removeAt(int index) {
    Recorder::begin("Remove at index (dynamic array)", CODE_REMOVE);
    if (index < 0 || index >= count) {
        snap(data, capacity, count, plain(capacity), "Index " + to_string(index) + " does not exist (valid: 0 to " + to_string(count - 1) + ").", 0, vector<Marker>());
        return false;
    }
    vector<Role> first = plain(capacity);
    first[index] = ROLE_REMOVED;
    snap(data, capacity, count, first, "Remove " + to_string(data[index]) + " at index " + to_string(index) + ". The gap must be closed.", 0, vector<Marker>());
    for (int i = index; i < count - 1; i++) {
        data[i] = data[i + 1]; // shift left to close the gap
        vector<Role> r = plain(capacity);
        r[i] = ROLE_NEW;
        snap(data, capacity, count, r, "Shift " + to_string(data[i]) + " one place to the left, into slot " + to_string(i) + ".", 2, vector<Marker>());
    }
    Trace::say("shifted " + to_string(count - 1 - index) + " element(s) to the left");
    count--;
    snap(data, capacity, count, plain(capacity), "count goes down by one. Closing the gap made this O(n).", 3, vector<Marker>());
    return true;
}

bool DynamicArray::getAt(int index, int& value) const {
    if (index < 0 || index >= count) {
        return false;
    }
    value = data[index];
    return true;
}

int DynamicArray::linearSearch(int key) {
    Recorder::begin("Linear search", CODE_LINEAR);
    comparisons = 0;
    for (int i = 0; i < count; i++) {
        comparisons++;
        Trace::say("index " + to_string(i) + " holds " + to_string(data[i]));
        vector<Role> r = plain(capacity);
        r[i] = ROLE_CURRENT;
        if (data[i] == key) {
            r[i] = ROLE_FOUND;
            snap(data, capacity, count, r, "Found " + to_string(key) + " at index " + to_string(i) + " after " + to_string(comparisons) + " comparison(s).", 1, vector<Marker>{ Marker(i, "i") }, comparisons);
            return i;
        }
        snap(data, capacity, count, r, "Is " + to_string(data[i]) + " equal to " + to_string(key) + " ? No, check the next slot.", 1, vector<Marker>{ Marker(i, "i") }, comparisons);
    }
    snap(data, capacity, count, plain(capacity), "Checked all " + to_string(count) + " slots. " + to_string(key) + " is not in the array.", 3, vector<Marker>(), comparisons);
    return -1;
}

bool DynamicArray::isSorted() const {
    for (int i = 1; i < count; i++) {
        if (data[i - 1] > data[i]) {
            return false;
        }
    }
    return true;
}

int DynamicArray::binarySearch(int key) {
    Recorder::begin("Binary search", CODE_BINARY);
    comparisons = 0;
    if (!isSorted()) {
        snap(data, capacity, count, plain(capacity), "Binary search only works on sorted data, and this array is not sorted. Sort it first.", 0, vector<Marker>());
        return -2; // binary search only works on sorted data
    }
    int low = 0;
    int high = count - 1;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        comparisons++;
        Trace::say("low=" + to_string(low) + " high=" + to_string(high) + " mid=" + to_string(mid) + " value=" + to_string(data[mid]));
        vector<Role> r = plain(capacity);
        for (int i = 0; i < count; i++) if (i < low || i > high) r[i] = ROLE_DIM;
        r[mid] = ROLE_CURRENT;
        vector<Marker> m;
        m.push_back(Marker(low, "low"));
        m.push_back(Marker(high, "high"));
        m.push_back(Marker(mid, "mid"));
        snap(data, capacity, count, r, "Middle of [" + to_string(low) + " .. " + to_string(high) + "] is index " + to_string(mid) + ": " + to_string(data[mid]) + ". Compare with " + to_string(key) + ".", 2, m, comparisons);
        if (data[mid] == key) {
            r[mid] = ROLE_FOUND;
            snap(data, capacity, count, r, "Found " + to_string(key) + " at index " + to_string(mid) + " after only " + to_string(comparisons) + " comparison(s).", 3, m, comparisons);
            return mid;
        }
        if (data[mid] < key) {
            low = mid + 1;  // key can only be in the right half
            r[mid] = ROLE_KEPT;
            snap(data, capacity, count, r, to_string(data[mid]) + " < " + to_string(key) + ": the key can only be in the right half. Throw the left half away.", 4, m, comparisons);
        }
        else {
            high = mid - 1; // key can only be in the left half
            r[mid] = ROLE_KEPT;
            snap(data, capacity, count, r, to_string(data[mid]) + " > " + to_string(key) + ": the key can only be in the left half. Throw the right half away.", 5, m, comparisons);
        }
    }
    vector<Role> dim = plain(capacity);
    for (int i = 0; i < count; i++) dim[i] = ROLE_DIM;
    snap(data, capacity, count, dim, "The range is empty. " + to_string(key) + " is not in the array (" + to_string(comparisons) + " comparisons).", 6, vector<Marker>(), comparisons);
    return -1;
}

void DynamicArray::sortAscending() {
    Recorder::begin("Insertion sort (array)", CODE_SORT);
    snap(data, capacity, count, plain(capacity), "Sort the array with insertion sort so that binary search can be used.", 0, vector<Marker>());
    for (int i = 1; i < count; i++) {
        int key = data[i];
        int j = i - 1;
        while (j >= 0 && data[j] > key) {
            data[j + 1] = data[j];
            j--;
        }
        data[j + 1] = key;
        vector<Role> r = plain(capacity);
        for (int k = 0; k <= i; k++) r[k] = ROLE_DONE;
        r[j + 1] = ROLE_NEW;
        snap(data, capacity, count, r, "Inserted " + to_string(key) + " at index " + to_string(j + 1) + ". The first " + to_string(i + 1) + " values are sorted.", 3, vector<Marker>());
    }
    vector<Role> done = plain(capacity);
    for (int k = 0; k < count; k++) done[k] = ROLE_DONE;
    snap(data, capacity, count, done, "The array is sorted.", 4, vector<Marker>());
}

long DynamicArray::lastComparisons() const {
    return comparisons;
}

void DynamicArray::clear() {
    count = 0;
}

void DynamicArray::showArray(const string& message) const {
    Recorder::begin("Dynamic array", vector<string>());
    snap(data, capacity, count, plain(capacity), message, -1, vector<Marker>());
}

void DynamicArray::display() const {
    if (count == 0) {
        cout << "Array is empty." << endl;
        return;
    }
    cout << "[ ";
    for (int i = 0; i < count; i++) {
        cout << data[i] << " ";
    }
    cout << "]" << endl;
}
