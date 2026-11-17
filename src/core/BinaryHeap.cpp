#include "BinaryHeap.h"
#include "Trace.h"
#include "Recorder.h"
#include <iostream>
#include <string>
#include <vector>
using namespace std;

// ---------- recording helpers (used only when the recorder is switched on) ----------

namespace {
    const vector<string> CODE_HEAPSORT = {
        "for (i = n/2 - 1; i >= 0; i--) siftDown(i, n);   // build a max heap",
        "for (end = n - 1; end > 0; end--) {",
        "  swap(a[0], a[end]);        // largest goes to its final place",
        "  siftDown(0, end);          // restore the heap",
        "}"
    };
    const vector<string> CODE_HEAP_INSERT = {
        "arr[count++] = v;           // first free slot",
        "while (i > 0 && arr[i] beats arr[parent])",
        "  swap(arr[i], arr[parent]), i = parent;"
    };
    const vector<string> CODE_HEAP_EXTRACT = {
        "top = arr[0];",
        "arr[0] = arr[--count];      // last value goes to the top",
        "while (a child beats arr[i])",
        "  swap(arr[i], arr[bestChild]), i = bestChild;"
    };

    // bar view used by heap sort: the first `limit` bars are the heap, the rest is sorted
    struct HeapView {
        int a, b;
        bool moved, kept;
        int limit;
        HeapView() : a(-1), b(-1), moved(false), kept(false), limit(0) {}
    };

    void recordBars(const int arr[], int n, const string& message, int line, const HeapView& v, const SortStats& stats) {
        if (!Recorder::enabled()) return;
        Frame f;
        f.kind = KIND_BARS;
        f.message = message;
        f.codeLine = line;
        f.comparisons = stats.comparisons;
        f.swaps = stats.swaps;
        for (int i = 0; i < n; i++) {
            Role role = ROLE_NORMAL;
            if (i >= v.limit) role = ROLE_DONE;
            if (i == v.a || i == v.b) role = v.moved ? ROLE_NEW : (v.kept ? ROLE_KEPT : ROLE_CURRENT);
            f.cells.push_back(Cell(arr[i], role));
        }
        if (v.a >= 0) f.markers.push_back(Marker(v.a, "parent"));
        if (v.b >= 0 && v.b != v.a) f.markers.push_back(Marker(v.b, "child"));
        Recorder::add(f);
    }

    // a picture of a heap stored in arr[0..count-1]. The node id is the array index.
    void recordHeapTree(const int arr[], int count, const string& message, int line, int cur, int other, bool moved, bool kept, int added, long cmp) {
        if (!Recorder::enabled()) return;
        Frame f;
        f.kind = KIND_TREE;
        f.message = message;
        f.codeLine = line;
        f.comparisons = cmp;
        f.rootId = count > 0 ? 0 : -1;
        for (int i = 0; i < count; i++) {
            TreeNodeInfo t;
            t.id = i;
            t.value = arr[i];
            t.left = (2 * i + 1 < count) ? 2 * i + 1 : -1;
            t.right = (2 * i + 2 < count) ? 2 * i + 2 : -1;
            t.hasAux = true;
            t.aux = i; // the array index is drawn under the node
            if (i == cur || i == other) t.role = moved ? ROLE_NEW : (kept ? ROLE_KEPT : ROLE_CURRENT);
            if (i == added) t.role = ROLE_NEW;
            f.nodes.push_back(t);
        }
        Recorder::add(f);
    }
}

BinaryHeap::BinaryHeap(bool isMaxHeap) {
    maxHeap = isMaxHeap;
    count = 0;
}

bool BinaryHeap::isEmpty() const {
    return count == 0;
}

bool BinaryHeap::isFull() const {
    return count == MAX;
}

int BinaryHeap::size() const {
    return count;
}

bool BinaryHeap::isMaxHeap() const {
    return maxHeap;
}

bool BinaryHeap::comesBefore(int a, int b) const {
    return maxHeap ? a > b : a < b;
}

void BinaryHeap::siftUp(int index) {
    while (index > 0) {
        int parent = (index - 1) / 2;
        if (!comesBefore(arr[index], arr[parent])) {
            break;
        }
        int t = arr[index];
        arr[index] = arr[parent];
        arr[parent] = t;
        Trace::say("moved " + to_string(arr[parent]) + " up from index " + to_string(index) + " to " + to_string(parent));
        index = parent;
    }
}

void BinaryHeap::siftDown(int index) {
    while (true) {
        int left = 2 * index + 1;
        int right = 2 * index + 2;
        int best = index;
        if (left < count && comesBefore(arr[left], arr[best])) best = left;
        if (right < count && comesBefore(arr[right], arr[best])) best = right;
        if (best == index) {
            break;
        }
        int t = arr[index];
        arr[index] = arr[best];
        arr[best] = t;
        Trace::say("moved " + to_string(arr[best]) + " down from index " + to_string(index) + " to " + to_string(best));
        index = best;
    }
}

bool BinaryHeap::insert(int value) {
    Recorder::begin(maxHeap ? "Insert into the max heap" : "Insert into the min heap", CODE_HEAP_INSERT);
    if (isFull()) {
        recordHeapTree(arr, count, "The heap is full (" + to_string(MAX) + " values). Nothing is inserted.", 0, -1, -1, false, false, -1, 0);
        return false;
    }
    arr[count] = value; // put the new value in the first free slot ...
    count++;
    Trace::say("placed " + to_string(value) + " at index " + to_string(count - 1));
    recordHeapTree(arr, count, "Put " + to_string(value) + " in the first free slot, index " + to_string(count - 1) + ".", 0, -1, -1, false, false, count - 1, 0);

    // ... then let it float up to its place
    int index = count - 1;
    long cmp = 0;
    while (index > 0) {
        int parent = (index - 1) / 2;
        cmp++;
        recordHeapTree(arr, count, "Does " + to_string(arr[index]) + " beat its parent " + to_string(arr[parent]) + " ?", 1, index, parent, false, false, -1, cmp);
        if (!comesBefore(arr[index], arr[parent])) {
            recordHeapTree(arr, count, "No. The heap rule holds, the value stays here.", 1, index, parent, false, true, -1, cmp);
            return true;
        }
        int t = arr[index];
        arr[index] = arr[parent];
        arr[parent] = t;
        Trace::say("moved " + to_string(arr[parent]) + " up from index " + to_string(index) + " to " + to_string(parent));
        recordHeapTree(arr, count, "Yes. Swap them: the new value moves up.", 2, index, parent, true, false, -1, cmp);
        index = parent;
    }
    recordHeapTree(arr, count, "The value reached the top. Done.", 2, -1, -1, false, false, -1, cmp);
    return true;
}

bool BinaryHeap::extractTop(int& value) {
    Recorder::begin(maxHeap ? "Extract the max" : "Extract the min", CODE_HEAP_EXTRACT);
    if (isEmpty()) {
        recordHeapTree(arr, count, "The heap is empty.", 0, -1, -1, false, false, -1, 0);
        return false;
    }
    value = arr[0];
    recordHeapTree(arr, count, "The top value " + to_string(value) + " is the one that leaves.", 0, 0, -1, false, false, -1, 0);
    count--;
    arr[0] = arr[count]; // the last value goes to the top ...
    Trace::say("moved the last value " + to_string(arr[0]) + " to the top");
    if (count == 0) {
        recordHeapTree(arr, count, "The heap is empty now.", 1, -1, -1, false, false, -1, 0);
        return true;
    }
    recordHeapTree(arr, count, "The last value " + to_string(arr[0]) + " moves to the top.", 1, 0, -1, false, false, -1, 0);

    // ... and sinks down to its place
    int index = 0;
    long cmp = 0;
    while (true) {
        int left = 2 * index + 1;
        int right = 2 * index + 2;
        int best = index;
        if (left < count) { cmp++; if (comesBefore(arr[left], arr[best])) best = left; }
        if (right < count) { cmp++; if (comesBefore(arr[right], arr[best])) best = right; }
        if (best == index) {
            recordHeapTree(arr, count, "Both children are behind " + to_string(arr[index]) + ". The heap rule holds again.", 2, index, -1, false, true, -1, cmp);
            break;
        }
        int t = arr[index];
        arr[index] = arr[best];
        arr[best] = t;
        Trace::say("moved " + to_string(arr[best]) + " down from index " + to_string(index) + " to " + to_string(best));
        recordHeapTree(arr, count, "Swap with the better child: the value sinks down.", 3, index, best, true, false, -1, cmp);
        index = best;
    }
    return true;
}

bool BinaryHeap::peek(int& value) const {
    if (isEmpty()) {
        return false;
    }
    value = arr[0];
    return true;
}

bool BinaryHeap::buildHeap(const int a[], int n) {
    if (n < 0 || n > MAX) {
        return false;
    }
    for (int i = 0; i < n; i++) {
        arr[i] = a[i];
    }
    count = n;
    for (int i = count / 2 - 1; i >= 0; i--) { // start from the last node that has a child
        siftDown(i);
    }
    return true;
}

bool BinaryHeap::getAt(int index, int& value) const {
    if (index < 0 || index >= count) {
        return false;
    }
    value = arr[index];
    return true;
}

bool BinaryHeap::isValidHeap() const {
    for (int i = 1; i < count; i++) {
        int parent = (i - 1) / 2;
        if (comesBefore(arr[i], arr[parent])) {
            return false;
        }
    }
    return true;
}

void BinaryHeap::clear() {
    count = 0;
}

void BinaryHeap::display() const {
    if (count == 0) {
        cout << "Heap is empty." << endl;
        return;
    }
    cout << "Array: [ ";
    for (int i = 0; i < count; i++) {
        cout << arr[i] << " ";
    }
    cout << "]" << endl;

    int index = 0;
    int levelSize = 1;
    int level = 0;
    while (index < count) {
        cout << "Level " << level << ": ";
        for (int k = 0; k < levelSize && index < count; k++) {
            cout << arr[index] << " ";
            index++;
        }
        cout << endl;
        levelSize *= 2;
        level++;
    }
}

// sinks a[index] down inside the first n elements of a max heap (total = size of the whole array)
static void siftDownMax(int a[], int total, int n, int index, SortStats& stats) {
    while (true) {
        int left = 2 * index + 1;
        int right = 2 * index + 2;
        int largest = index;
        HeapView v;
        v.limit = n;
        v.a = index;
        if (left < n) {
            stats.comparisons++;
            v.b = left;
            recordBars(a, total, "Compare parent " + to_string(a[index]) + " with its left child " + to_string(a[left]) + ".", 3, v, stats);
            if (a[left] > a[largest]) largest = left;
        }
        if (right < n) {
            stats.comparisons++;
            v.b = right;
            recordBars(a, total, "Compare the larger so far (" + to_string(a[largest]) + ") with the right child " + to_string(a[right]) + ".", 3, v, stats);
            if (a[right] > a[largest]) largest = right;
        }
        if (largest == index) {
            v.b = -1;
            v.kept = true;
            recordBars(a, total, to_string(a[index]) + " is not smaller than its children. The heap rule holds here.", 3, v, stats);
            break;
        }
        int t = a[index];
        a[index] = a[largest];
        a[largest] = t;
        stats.swaps++;
        v.b = largest;
        v.moved = true;
        recordBars(a, total, "Swap the parent with its larger child " + to_string(a[index]) + ".", 3, v, stats);
        index = largest;
    }
}

void heapSort(int a[], int n, SortStats& stats) {
    Recorder::begin("Heap sort", CODE_HEAPSORT);
    {
        HeapView v;
        v.limit = n;
        recordBars(a, n, "Start. First turn the array into a max heap (largest value on top).", 0, v, stats);
    }
    for (int i = n / 2 - 1; i >= 0; i--) { // build a max heap inside the array
        siftDownMax(a, n, n, i, stats);
    }
    {
        HeapView v;
        v.limit = n;
        v.a = 0;
        recordBars(a, n, "Max heap built. The largest value is at index 0.", 0, v, stats);
    }
    Trace::say("max heap built, the largest value is at index 0");
    for (int end = n - 1; end > 0; end--) {
        int t = a[0];  // move the largest value to its final place at the end
        a[0] = a[end];
        a[end] = t;
        stats.swaps++;
        Trace::say("moved " + to_string(a[end]) + " to index " + to_string(end));
        HeapView v;
        v.limit = end;
        v.a = 0;
        v.moved = true;
        recordBars(a, n, "Swap the largest value " + to_string(a[end]) + " to its final place at index " + to_string(end) + ".", 2, v, stats);
        siftDownMax(a, n, end, 0, stats);
    }
    HeapView fin;
    recordBars(a, n, "Sorted. " + to_string(stats.comparisons) + " comparisons and " + to_string(stats.swaps) + " swaps.", -1, fin, stats);
}
