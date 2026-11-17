#include "Sorting.h"
#include "Trace.h"
#include "Recorder.h"
#include <string>
#include <vector>
using namespace std;

// ---------- recording helpers (used only when the recorder is switched on) ----------

namespace {
    const vector<string> CODE_BUBBLE = {
        "for (i = 0; i < n - 1; i++) {",
        "  for (j = 0; j < n - 1 - i; j++)",
        "    if (a[j] > a[j + 1])",
        "      swap(a[j], a[j + 1]);",
        "}"
    };
    const vector<string> CODE_SELECTION = {
        "for (i = 0; i < n - 1; i++) {",
        "  min = i;",
        "  for (j = i + 1; j < n; j++)",
        "    if (a[j] < a[min]) min = j;",
        "  swap(a[i], a[min]);",
        "}"
    };
    const vector<string> CODE_INSERTION = {
        "for (i = 1; i < n; i++) {",
        "  key = a[i]; j = i - 1;",
        "  while (j >= 0 && a[j] > key) {",
        "    a[j + 1] = a[j]; j--;",
        "  }",
        "  a[j + 1] = key;",
        "}"
    };
    const vector<string> CODE_MERGE = {
        "mergeSort(a, left, right) {",
        "  if (left >= right) return;",
        "  mid = (left + right) / 2;",
        "  mergeSort(a, left, mid);",
        "  mergeSort(a, mid + 1, right);",
        "  merge(a, left, mid, right);",
        "}"
    };
    const vector<string> CODE_QUICK = {
        "quickSort(a, low, high) {",
        "  if (low >= high) return;",
        "  p = partition(a, low, high);",
        "  quickSort(a, low, p - 1);",
        "  quickSort(a, p + 1, high);",
        "}"
    };

    // which bars are highlighted in a frame
    struct ArrayView {
        int a;          // the two positions being compared or moved
        int b;
        bool moved;     // true when a and b were just swapped or shifted (condition met, shown green)
        bool kept;      // true when a and b were compared and the condition was NOT met (shown rose)
        int rangeLow;   // part of the array that is being worked on (merge sort), -1 for none
        int rangeHigh;
        Role rangeRole;
        int pivot;      // position of a pivot / current minimum, -1 for none
        const vector<char>* done; // positions that are in their final place
        string labelA;  // small tags drawn above the bars
        string labelB;
        string labelPivot;

        ArrayView() : a(-1), b(-1), moved(false), kept(false), rangeLow(-1), rangeHigh(-1), rangeRole(ROLE_CURRENT),
                      pivot(-1), done(nullptr) {}
        void reset() {
            a = -1; b = -1; moved = false; kept = false; pivot = -1;
            labelA = ""; labelB = ""; labelPivot = "";
        }
    };

    void recordArray(const int arr[], int n, const string& message, int line, const ArrayView& view, const SortStats& stats) {
        if (!Recorder::enabled()) {
            return;
        }
        Frame f;
        f.kind = KIND_BARS;
        f.message = message;
        f.codeLine = line;
        f.comparisons = stats.comparisons;
        f.swaps = stats.swaps;
        for (int i = 0; i < n; i++) {
            Role role = ROLE_NORMAL;
            if (view.rangeLow >= 0 && i >= view.rangeLow && i <= view.rangeHigh) role = view.rangeRole;
            if (view.done != nullptr && (*view.done)[i]) role = ROLE_DONE;
            if (i == view.a || i == view.b) role = view.moved ? ROLE_NEW : (view.kept ? ROLE_KEPT : ROLE_CURRENT);
            if (i == view.pivot) role = ROLE_FOUND;
            f.cells.push_back(Cell(arr[i], role));
        }
        if (view.a >= 0 && !view.labelA.empty()) f.markers.push_back(Marker(view.a, view.labelA));
        if (view.b >= 0 && !view.labelB.empty()) f.markers.push_back(Marker(view.b, view.labelB));
        if (view.pivot >= 0 && !view.labelPivot.empty()) f.markers.push_back(Marker(view.pivot, view.labelPivot));
        Recorder::add(f);
    }

    void recordFinished(const int arr[], int n, const SortStats& stats) {
        vector<char> all(n > 0 ? n : 1, 1);
        ArrayView view;
        view.done = &all;
        recordArray(arr, n, "Sorted. " + to_string(stats.comparisons) + " comparisons and " + to_string(stats.swaps) + " swaps or moves.", -1, view, stats);
    }
}

// builds a text like "[ 5 1 4 ]" for step mode (only called when step mode is on)
static string arrayText(const int a[], int n) {
    string text = "[ ";
    for (int i = 0; i < n; i++) {
        text += to_string(a[i]) + " ";
    }
    return text + "]";
}

static void swapValues(int& x, int& y) {
    int t = x;
    x = y;
    y = t;
}

void bubbleSort(int a[], int n, SortStats& stats) {
    Recorder::begin("Bubble sort", CODE_BUBBLE);
    vector<char> done(n > 0 ? n : 1, 0);
    ArrayView view;
    view.done = &done;
    recordArray(a, n, "Start. Compare neighbours and swap the ones that are out of order.", 0, view, stats);

    for (int i = 0; i < n - 1; i++) {
        bool swapped = false;
        for (int j = 0; j < n - 1 - i; j++) {
            stats.comparisons++;
            view.reset();
            view.a = j;
            view.b = j + 1;
            view.labelA = "j";
            view.labelB = "j+1";
            recordArray(a, n, "Is " + to_string(a[j]) + " > " + to_string(a[j + 1]) + " ?", 2, view, stats);
            if (a[j] > a[j + 1]) {
                swapValues(a[j], a[j + 1]);
                stats.swaps++;
                swapped = true;
                view.moved = true;
                recordArray(a, n, "Yes, out of order. Swapped them.", 3, view, stats);
                if (Trace::enabled) Trace::say("swap positions " + to_string(j) + " and " + to_string(j + 1) + " -> " + arrayText(a, n));
            }
            else {
                view.kept = true;
                recordArray(a, n, "No, " + to_string(a[j]) + " <= " + to_string(a[j + 1]) + ". Already in order, keep them.", 2, view, stats);
            }
        }
        done[n - 1 - i] = 1;
        view.reset();
        recordArray(a, n, "Pass " + to_string(i + 1) + " finished: the largest value is in its final place.", 0, view, stats);
        if (Trace::enabled) Trace::say("pass " + to_string(i + 1) + " done: " + arrayText(a, n));
        if (!swapped) {
            break; // no swaps in a whole pass means the array is sorted
        }
    }
    recordFinished(a, n, stats);
}

void selectionSort(int a[], int n, SortStats& stats) {
    Recorder::begin("Selection sort", CODE_SELECTION);
    vector<char> done(n > 0 ? n : 1, 0);
    ArrayView view;
    view.done = &done;
    recordArray(a, n, "Start. Find the smallest value of the rest and put it in front.", 0, view, stats);

    for (int i = 0; i < n - 1; i++) {
        int minIndex = i;
        view.reset();
        view.pivot = minIndex;
        view.labelPivot = "min";
        recordArray(a, n, "Assume position " + to_string(i) + " holds the smallest value (" + to_string(a[i]) + ").", 1, view, stats);
        for (int j = i + 1; j < n; j++) {
            stats.comparisons++;
            view.reset();
            view.a = j;
            view.labelA = "j";
            view.pivot = minIndex;
            view.labelPivot = "min";
            recordArray(a, n, "Is " + to_string(a[j]) + " < " + to_string(a[minIndex]) + " ?", 3, view, stats);
            if (a[j] < a[minIndex]) {
                minIndex = j;
                view.reset();
                view.a = j;
                view.moved = true;
                view.labelA = "new min";
                recordArray(a, n, "Yes, " + to_string(a[j]) + " is smaller: it is the new smallest value.", 3, view, stats);
            }
            else {
                view.kept = true;
                recordArray(a, n, "No, " + to_string(a[j]) + " is not smaller than " + to_string(a[minIndex]) + ". Keep the current minimum.", 3, view, stats);
            }
        }
        if (minIndex != i) {
            swapValues(a[i], a[minIndex]);
            stats.swaps++;
            view.reset();
            view.a = i;
            view.b = minIndex;
            view.moved = true;
            recordArray(a, n, "Swap the smallest value into position " + to_string(i) + ".", 4, view, stats);
        }
        done[i] = 1;
        view.reset();
        recordArray(a, n, "Position " + to_string(i) + " is final.", 4, view, stats);
        if (Trace::enabled) Trace::say("smallest of the rest is at " + to_string(minIndex) + ", placed at " + to_string(i) + " -> " + arrayText(a, n));
    }
    recordFinished(a, n, stats);
}

void insertionSort(int a[], int n, SortStats& stats) {
    Recorder::begin("Insertion sort", CODE_INSERTION);
    vector<char> done(n > 0 ? n : 1, 0);
    if (n > 0) {
        done[0] = 1; // one element is sorted by itself
    }
    ArrayView view;
    view.done = &done;
    recordArray(a, n, "Start. The first element alone is a sorted part. Insert the others one by one.", 0, view, stats);

    for (int i = 1; i < n; i++) {
        int key = a[i];
        int j = i - 1;
        view.reset();
        view.pivot = i;
        view.labelPivot = "key";
        recordArray(a, n, "Take " + to_string(key) + " and find its place in the sorted part on the left.", 1, view, stats);
        while (j >= 0) {
            stats.comparisons++;
            view.reset();
            view.a = j;
            view.labelA = "j";
            recordArray(a, n, "Is " + to_string(a[j]) + " > " + to_string(key) + " ?", 2, view, stats);
            if (a[j] > key) {
                a[j + 1] = a[j];
                stats.swaps++;
                view.moved = true;
                view.b = j + 1;
                recordArray(a, n, "Yes. Shift " + to_string(a[j]) + " one place to the right.", 3, view, stats);
                j--;
            }
            else {
                view.kept = true;
                recordArray(a, n, "No, " + to_string(a[j]) + " is not greater than " + to_string(key) + ". Stop here.", 2, view, stats);
                break;
            }
        }
        a[j + 1] = key;
        for (int k = 0; k <= i; k++) done[k] = 1;
        view.reset();
        view.a = j + 1;
        view.moved = true;
        recordArray(a, n, "Place " + to_string(key) + " at position " + to_string(j + 1) + ".", 5, view, stats);
        if (Trace::enabled) Trace::say("inserted " + to_string(key) + " at " + to_string(j + 1) + " -> " + arrayText(a, n));
    }
    recordFinished(a, n, stats);
}

static void merge(int a[], int temp[], int n, int left, int mid, int right, SortStats& stats) {
    ArrayView view;
    view.rangeLow = left;
    view.rangeHigh = right;
    recordArray(a, n, "Merge the sorted halves " + to_string(left) + " to " + to_string(mid) + " and " + to_string(mid + 1) + " to " + to_string(right) + ".", 5, view, stats);

    int i = left;
    int j = mid + 1;
    int k = left;
    while (i <= mid && j <= right) {
        stats.comparisons++;
        ArrayView step;
        step.a = i;
        step.b = j;
        step.labelA = "L";
        step.labelB = "R";
        recordArray(a, n, "Compare the fronts of both halves: " + to_string(a[i]) + " and " + to_string(a[j]) + ".", 5, step, stats);
        int take;
        if (a[i] <= a[j]) {
            take = i;
            temp[k++] = a[i++];
        }
        else {
            take = j;
            temp[k++] = a[j++];
        }
        stats.swaps++;
        step.a = take;
        step.b = -1;
        step.moved = true;
        step.labelB = "";
        recordArray(a, n, to_string(temp[k - 1]) + " is the smaller one and goes to the merged result.", 5, step, stats);
    }
    while (i <= mid) {
        temp[k++] = a[i++];
        stats.swaps++;
    }
    while (j <= right) {
        temp[k++] = a[j++];
        stats.swaps++;
    }
    for (int x = left; x <= right; x++) {
        a[x] = temp[x];
    }
    view.rangeRole = ROLE_NEW;
    recordArray(a, n, "Positions " + to_string(left) + " to " + to_string(right) + " are now sorted.", 5, view, stats);
    if (Trace::enabled) Trace::say("merged positions " + to_string(left) + " to " + to_string(right));
}

static void mergeSortRange(int a[], int temp[], int n, int left, int right, SortStats& stats) {
    if (left >= right) {
        return;
    }
    int mid = left + (right - left) / 2;
    mergeSortRange(a, temp, n, left, mid, stats);
    mergeSortRange(a, temp, n, mid + 1, right, stats);
    merge(a, temp, n, left, mid, right, stats);
}

void mergeSort(int a[], int n, SortStats& stats) {
    Recorder::begin("Merge sort", CODE_MERGE);
    ArrayView start;
    recordArray(a, n, "Start. Split in halves until one element is left, then merge the sorted halves.", 0, start, stats);
    if (n < 2) {
        recordFinished(a, n, stats);
        return;
    }
    int* temp = new int[n];
    mergeSortRange(a, temp, n, 0, n - 1, stats);
    delete[] temp;
    recordFinished(a, n, stats);
}

static int partition(int a[], int n, int low, int high, SortStats& stats, vector<char>& done) {
    int pivot = a[high]; // the last element is the pivot
    int i = low - 1;
    ArrayView view;
    view.done = &done;
    view.pivot = high;
    view.labelPivot = "pivot";
    recordArray(a, n, "Pick the last value of this range, " + to_string(pivot) + ", as the pivot.", 2, view, stats);
    for (int j = low; j < high; j++) {
        stats.comparisons++;
        view.a = j;
        view.b = -1;
        view.moved = false;
        view.kept = false;
        view.labelA = "j";
        recordArray(a, n, "Is " + to_string(a[j]) + " < " + to_string(pivot) + " ?", 2, view, stats);
        if (a[j] < pivot) {
            i++;
            if (i != j) {
                swapValues(a[i], a[j]);
                stats.swaps++;
            }
            view.a = i;
            view.b = (i != j) ? j : -1;
            view.moved = true;
            view.labelA = "i";
            recordArray(a, n, "Yes. It belongs on the left side, move it to position " + to_string(i) + ".", 2, view, stats);
        }
        else {
            view.kept = true;
            recordArray(a, n, "No, " + to_string(a[j]) + " >= " + to_string(pivot) + ". It stays on the right side.", 2, view, stats);
        }
    }
    if (i + 1 != high) {
        swapValues(a[i + 1], a[high]);
        stats.swaps++;
    }
    done[i + 1] = 1;
    ArrayView fin;
    fin.done = &done;
    recordArray(a, n, "Pivot " + to_string(pivot) + " is placed at position " + to_string(i + 1) + ". Smaller values are on its left, larger ones on its right.", 2, fin, stats);
    return i + 1;
}

static void quickSortRange(int a[], int n, int low, int high, SortStats& stats, vector<char>& done) {
    if (low > high) {
        return;
    }
    if (low == high) {
        done[low] = 1;
        return;
    }
    int p = partition(a, n, low, high, stats, done);
    if (Trace::enabled) Trace::say("pivot placed at index " + to_string(p) + " (range " + to_string(low) + " to " + to_string(high) + ")");
    quickSortRange(a, n, low, p - 1, stats, done);
    quickSortRange(a, n, p + 1, high, stats, done);
}

void quickSort(int a[], int n, SortStats& stats) {
    Recorder::begin("Quick sort", CODE_QUICK);
    vector<char> done(n > 0 ? n : 1, 0);
    ArrayView start;
    start.done = &done;
    recordArray(a, n, "Start. Pick a pivot, put it in its final place, then sort the two sides.", 0, start, stats);
    quickSortRange(a, n, 0, n - 1, stats, done);
    recordFinished(a, n, stats);
}

bool isSortedAscending(const int a[], int n) {
    for (int i = 1; i < n; i++) {
        if (a[i - 1] > a[i]) {
            return false;
        }
    }
    return true;
}
