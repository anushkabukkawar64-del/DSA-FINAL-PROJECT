// ============================================================
// Stack.h - Transaction Undo Stack (Linked List Implementation)
// Smart Stock Market Platform - Indian Stock Market
// B.Tech CSE 2025-29 | DSA with C++ | Sem II | ITM Skills University
// ============================================================

#ifndef STACK_H
#define STACK_H

#include "Stock.h"
#include <iostream>
#include <iomanip>

using namespace std;

// ─────────────────────────────────────────────
// Stack Node
// ─────────────────────────────────────────────
struct StackNode {
    Transaction data;
    StackNode*  next;
    StackNode(Transaction t) : data(t), next(nullptr) {}
};

// ─────────────────────────────────────────────
// TransactionStack — LIFO Stack for Undo
// Stores last N buy/sell actions
// ─────────────────────────────────────────────
class TransactionStack {
private:
    StackNode* top;
    int        count;

public:
    TransactionStack() : top(nullptr), count(0) {}

    ~TransactionStack() {
        while (!isEmpty()) pop();
    }

    // Push a transaction (after buy/sell)
    void push(const Transaction& t) {
        StackNode* node = new StackNode(t);
        node->next = top;
        top        = node;
        count++;
        cout << "  [STACK] Pushed: " << t.type << " " << t.quantity
             << " × " << t.symbol << " @ ₹" << fixed << setprecision(2) << t.price << "\n";
    }

    // Pop for undo
    Transaction pop() {
        if (isEmpty()) {
            cout << "  [STACK] ✗ Stack is empty — nothing to undo.\n";
            return Transaction();
        }
        StackNode*  temp = top;
        Transaction data = top->data;
        top  = top->next;
        delete temp;
        count--;
        return data;
    }

    // Peek at top without removing
    Transaction peek() const {
        if (isEmpty()) {
            cout << "  [STACK] ✗ Stack is empty.\n";
            return Transaction();
        }
        return top->data;
    }

    bool isEmpty() const { return top == nullptr; }
    int  size()    const { return count; }

    // Display full stack (top to bottom)
    void display() const {
        if (isEmpty()) {
            cout << "  [STACK] Empty — No transactions recorded.\n";
            return;
        }
        cout << "\n  ┌────── TRANSACTION HISTORY (UNDO STACK) ──────┐\n";
        cout << "  │  (Top = most recent)                          │\n";
        cout << "  ├──────────────────────────────────────────────┤\n";
        StackNode* curr = top;
        int i = 1;
        while (curr) {
            string sign = (curr->data.type == "BUY") ? "🟢 BUY " : "🔴 SELL";
            cout << "  │  " << setw(2) << i++ << ". " << sign << "  "
                 << setw(12) << curr->data.symbol
                 << "  Qty: " << setw(4) << curr->data.quantity
                 << "  @ ₹" << fixed << setprecision(2) << curr->data.price
                 << "  │\n";
            curr = curr->next;
        }
        cout << "  └──────────────────────────────────────────────┘\n";
    }
};

#endif // STACK_H
