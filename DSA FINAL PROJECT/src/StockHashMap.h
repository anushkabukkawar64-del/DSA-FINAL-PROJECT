// ============================================================
// StockHashMap.h - Custom Hash Map for O(1) Stock Lookup
// Smart Stock Market Platform - Indian Stock Market
// B.Tech CSE 2025-29 | DSA with C++ | Sem II | ITM Skills University
// ============================================================

#ifndef STOCKHASHMAP_H
#define STOCKHASHMAP_H

#include "Stock.h"
#include <string>
#include <iostream>

using namespace std;

// ─────────────────────────────────────────────
// Hash Node — each bucket holds a linked list
// ─────────────────────────────────────────────
struct HashNode {
    string key;       // NSE symbol e.g. "RELIANCE"
    Stock  value;     // The stock object
    HashNode* next;   // Chaining for collision handling

    HashNode(string k, Stock v) : key(k), value(v), next(nullptr) {}
};

// ─────────────────────────────────────────────
// StockHashMap — Separate Chaining Hash Table
// Capacity: 23 (prime number, reduces collisions)
// ─────────────────────────────────────────────
class StockHashMap {
private:
    static const int CAPACITY = 23;
    HashNode* table[CAPACITY];
    int       size;

    // Hash function: polynomial rolling hash on symbol characters
    int hashFunction(const string& key) const {
        int hash = 0;
        int prime = 31;
        int mod   = CAPACITY;
        for (int i = 0; i < (int)key.size(); i++) {
            hash = (hash * prime + (int)key[i]) % mod;
        }
        return (hash + mod) % mod;   // ensure non-negative
    }

public:
    StockHashMap() : size(0) {
        for (int i = 0; i < CAPACITY; i++)
            table[i] = nullptr;
    }

    ~StockHashMap() {
        for (int i = 0; i < CAPACITY; i++) {
            HashNode* curr = table[i];
            while (curr) {
                HashNode* temp = curr;
                curr = curr->next;
                delete temp;
            }
        }
    }

    // Insert or update a stock
    void insert(const string& symbol, const Stock& stock) {
        int idx = hashFunction(symbol);
        HashNode* curr = table[idx];

        // Check if key already exists → update
        while (curr) {
            if (curr->key == symbol) {
                curr->value = stock;
                return;
            }
            curr = curr->next;
        }

        // New node — prepend to chain
        HashNode* newNode = new HashNode(symbol, stock);
        newNode->next = table[idx];
        table[idx]    = newNode;
        size++;
    }

    // Search — O(1) average case
    Stock* search(const string& symbol) const {
        int idx = hashFunction(symbol);
        HashNode* curr = table[idx];
        while (curr) {
            if (curr->key == symbol)
                return &curr->value;
            curr = curr->next;
        }
        return nullptr;  // not found
    }

    // Remove a stock
    bool remove(const string& symbol) {
        int idx = hashFunction(symbol);
        HashNode* curr = table[idx];
        HashNode* prev = nullptr;

        while (curr) {
            if (curr->key == symbol) {
                if (prev) prev->next = curr->next;
                else       table[idx] = curr->next;
                delete curr;
                size--;
                return true;
            }
            prev = curr;
            curr = curr->next;
        }
        return false;
    }

    // Update price of an existing stock
    bool updatePrice(const string& symbol, double newPrice) {
        Stock* s = search(symbol);
        if (!s) return false;
        s->price = newPrice;
        return true;
    }

    // Return all stocks as a vector-like array (for sorting/display)
    // Caller must delete[] the array
    Stock* getAllStocks(int& count) const {
        count = 0;
        Stock* arr = new Stock[size];
        for (int i = 0; i < CAPACITY; i++) {
            HashNode* curr = table[i];
            while (curr) {
                arr[count++] = curr->value;
                curr = curr->next;
            }
        }
        return arr;
    }

    int getSize() const { return size; }

    // Display the hash table with bucket info (for DSA demonstration)
    void displayTable() const {
        cout << "\n╔══ HASH TABLE (Separate Chaining) ══╗\n";
        cout << "   Capacity: " << CAPACITY << " buckets\n";
        for (int i = 0; i < CAPACITY; i++) {
            if (table[i]) {
                cout << "  [" << setw(2) << i << "] → ";
                HashNode* curr = table[i];
                while (curr) {
                    cout << curr->key;
                    if (curr->next) cout << " → ";
                    curr = curr->next;
                }
                cout << "\n";
            }
        }
        cout << "╚════════════════════════════════════╝\n";
    }
};

#endif // STOCKHASHMAP_H
