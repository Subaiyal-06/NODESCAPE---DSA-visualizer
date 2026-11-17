// Sorting algorithms on int arrays. Every function counts its comparisons and swaps
// (for merge sort a "swap" is one element copied into place).

#ifndef SORTING_H
#define SORTING_H

struct SortStats {
    long comparisons = 0;
    long swaps = 0;
};

void bubbleSort(int a[], int n, SortStats& stats);
void selectionSort(int a[], int n, SortStats& stats);
void insertionSort(int a[], int n, SortStats& stats);
void mergeSort(int a[], int n, SortStats& stats);
void quickSort(int a[], int n, SortStats& stats);

bool isSortedAscending(const int a[], int n);

#endif
