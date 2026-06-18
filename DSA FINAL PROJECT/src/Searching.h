// ============================================================
// Searching.h - Linear & Binary Search for Stocks
// Smart Stock Market Platform - Indian Stock Market
// B.Tech CSE 2025-29 | DSA with C++ | Sem II | ITM Skills University
// ============================================================

#ifndef SEARCHING_H
#define SEARCHING_H

#include "Stock.h"
#include <string>
#include <iostream>
#include <algorithm>

using namespace std;

// ─────────────────────────────────────────────
// Searching Algorithms on Stock arrays
// ─────────────────────────────────────────────

class Searching {
public:

    // ══════════════════════════════════════════
    // 1. LINEAR SEARCH — by Company Name
    //    O(n) — works on unsorted array
    //    Supports partial, case-insensitive match
    // ══════════════════════════════════════════
    static int linearSearchByName(Stock arr[], int n, const string& query) {
        string q = toLower(query);
        for (int i = 0; i < n; i++) {
            if (toLower(arr[i].company).find(q) != string::npos ||
                toLower(arr[i].symbol).find(q) != string::npos) {
                return i;  // Return first match index
            }
        }
        return -1;  // Not found
    }

    // ══════════════════════════════════════════
    // 2. LINEAR SEARCH — by Sector
    //    Returns all stocks in that sector
    // ══════════════════════════════════════════
    static void linearSearchBySector(Stock arr[], int n, const string& sector) {
        string q = toLower(sector);
        bool found = false;
        cout << "\n  Stocks in Sector: \"" << sector << "\"\n";
        cout << "  ─────────────────────────────────────────────\n";
        for (int i = 0; i < n; i++) {
            if (toLower(arr[i].sector).find(q) != string::npos) {
                cout << "  ";
                arr[i].printSummary();
                found = true;
            }
        }
        if (!found)
            cout << "  ⚠ No stocks found in sector: " << sector << "\n";
    }

    // ══════════════════════════════════════════
    // 3. BINARY SEARCH — by Symbol (exact match)
    //    O(log n) — array must be SORTED by symbol
    //    Use after mergeSortBySymbol()
    // ══════════════════════════════════════════
    static int binarySearchBySymbol(Stock arr[], int n, const string& symbol) {
        int low = 0, high = n - 1;
        string target = toUpper(symbol);

        while (low <= high) {
            int mid = low + (high - low) / 2;
            string midSym = toUpper(arr[mid].symbol);

            if (midSym == target)
                return mid;
            else if (midSym < target)
                low = mid + 1;
            else
                high = mid - 1;
        }
        return -1;  // Not found
    }

    // ══════════════════════════════════════════
    // 4. PRICE RANGE SEARCH — Linear
    //    Find all stocks within ₹ range
    // ══════════════════════════════════════════
    static void searchByPriceRange(Stock arr[], int n, double minPrice, double maxPrice) {
        bool found = false;
        cout << "\n  Stocks between ₹" << minPrice << " – ₹" << maxPrice << "\n";
        cout << "  ─────────────────────────────────────────────\n";
        for (int i = 0; i < n; i++) {
            if (arr[i].price >= minPrice && arr[i].price <= maxPrice) {
                cout << "  ";
                arr[i].printSummary();
                found = true;
            }
        }
        if (!found)
            cout << "  ⚠ No stocks in that price range.\n";
    }

    // Display search result nicely
    static void displayResult(Stock arr[], int idx, const string& query) {
        if (idx == -1) {
            cout << "\n  ✗ Stock not found: \"" << query << "\"\n";
        } else {
            cout << "\n  ✓ Found:";
            arr[idx].printDetail();
        }
    }

private:
    static string toLower(const string& s) {
        string res = s;
        for (char& c : res) c = tolower(c);
        return res;
    }

    static string toUpper(const string& s) {
        string res = s;
        for (char& c : res) c = toupper(c);
        return res;
    }
};

#endif // SEARCHING_H
