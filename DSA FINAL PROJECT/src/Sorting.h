// ============================================================
// Sorting.h - All Sorting Algorithms for Stock Data
// Smart Stock Market Platform - Indian Stock Market
// B.Tech CSE 2025-29 | DSA with C++ | Sem II | ITM Skills University
// ============================================================

#ifndef SORTING_H
#define SORTING_H

#include "Stock.h"
#include <iostream>
#include <iomanip>
#include <string>

using namespace std;

// ─────────────────────────────────────────────
// Sorting Utilities for Stock[] arrays
// Implements: Bubble, Selection, Merge, Quick Sort
// ─────────────────────────────────────────────

class Sorting {
public:

    // ══════════════════════════════════════════
    // 1. BUBBLE SORT — by Price (ascending)
    //    O(n²) — great for demonstration
    // ══════════════════════════════════════════
    static void bubbleSortByPrice(Stock arr[], int n, bool ascending = true) {
        for (int i = 0; i < n - 1; i++) {
            bool swapped = false;
            for (int j = 0; j < n - i - 1; j++) {
                bool condition = ascending
                    ? (arr[j].price > arr[j+1].price)
                    : (arr[j].price < arr[j+1].price);
                if (condition) {
                    swap(arr[j], arr[j+1]);
                    swapped = true;
                }
            }
            if (!swapped) break; // Optimized early exit
        }
    }

    // ══════════════════════════════════════════
    // 2. SELECTION SORT — by Gain % (ascending)
    //    O(n²) — min element selection
    // ══════════════════════════════════════════
    static void selectionSortByGain(Stock arr[], int n, bool ascending = true) {
        for (int i = 0; i < n - 1; i++) {
            int idx = i;
            for (int j = i + 1; j < n; j++) {
                bool condition = ascending
                    ? (arr[j].growthPct < arr[idx].growthPct)
                    : (arr[j].growthPct > arr[idx].growthPct);
                if (condition) idx = j;
            }
            if (idx != i) swap(arr[i], arr[idx]);
        }
    }

    // ══════════════════════════════════════════
    // 3. MERGE SORT — by Symbol (alphabetical)
    //    O(n log n) — efficient, stable
    // ══════════════════════════════════════════
    static void mergeSortBySymbol(Stock arr[], int left, int right) {
        if (left >= right) return;
        int mid = left + (right - left) / 2;
        mergeSortBySymbol(arr, left, mid);
        mergeSortBySymbol(arr, mid + 1, right);
        mergeBySymbol(arr, left, mid, right);
    }

    // ══════════════════════════════════════════
    // 4. QUICK SORT — by Volume / Performance
    //    O(n log n) avg — in-place, cache friendly
    // ══════════════════════════════════════════
    static void quickSortByVolume(Stock arr[], int low, int high) {
        if (low < high) {
            int pi = partitionByVolume(arr, low, high);
            quickSortByVolume(arr, low, pi - 1);
            quickSortByVolume(arr, pi + 1, high);
        }
    }

    // ──────────────────────────────────────────
    // Display sorted array with header
    // ──────────────────────────────────────────
    static void displaySorted(Stock arr[], int n, const string& label) {
        cout << "\n╔══════════════════════════════════════════════════════════════════╗\n";
        cout << "║  " << left << setw(64) << ("Sorted by: " + label) << "║\n";
        cout << "╠══════════════════════════════════════════════════════════════════╣\n";
        cout << "║ " << left << setw(12) << "Symbol"
             << setw(28) << "Company"
             << setw(14) << "Sector"
             << setw(10) << "Price(₹)"
             << setw(8)  << "Growth" << "║\n";
        cout << "╠══════════════════════════════════════════════════════════════════╣\n";
        for (int i = 0; i < n; i++) {
            string sign = (arr[i].growthPct >= 0) ? "+" : "";
            cout << "║ " << left
                 << setw(12) << arr[i].symbol
                 << setw(28) << arr[i].company
                 << setw(14) << arr[i].sector
                 << "₹" << right << setw(8) << fixed << setprecision(2) << arr[i].price
                 << "  " << left << sign << fixed << setprecision(1) << arr[i].growthPct << "%" << "  ║\n";
        }
        cout << "╚══════════════════════════════════════════════════════════════════╝\n";
    }

private:
    // Merge helper for merge sort
    static void mergeBySymbol(Stock arr[], int left, int mid, int right) {
        int n1 = mid - left + 1;
        int n2 = right - mid;

        Stock* L = new Stock[n1];
        Stock* R = new Stock[n2];

        for (int i = 0; i < n1; i++) L[i] = arr[left + i];
        for (int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];

        int i = 0, j = 0, k = left;
        while (i < n1 && j < n2) {
            if (L[i].symbol <= R[j].symbol)
                arr[k++] = L[i++];
            else
                arr[k++] = R[j++];
        }
        while (i < n1) arr[k++] = L[i++];
        while (j < n2) arr[k++] = R[j++];

        delete[] L;
        delete[] R;
    }

    // Partition helper for quick sort (by volume)
    static int partitionByVolume(Stock arr[], int low, int high) {
        long long pivot = arr[high].volume;
        int i = low - 1;
        for (int j = low; j < high; j++) {
            if (arr[j].volume <= pivot) {
                i++;
                swap(arr[i], arr[j]);
            }
        }
        swap(arr[i + 1], arr[high]);
        return i + 1;
    }
};

#endif // SORTING_H
