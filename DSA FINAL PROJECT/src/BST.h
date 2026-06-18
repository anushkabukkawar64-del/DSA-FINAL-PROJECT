// ============================================================
// BST.h - Binary Search Tree (by Stock Price)
// Smart Stock Market Platform - Indian Stock Market
// B.Tech CSE 2025-29 | DSA with C++ | Sem II | ITM Skills University
// ============================================================

#ifndef BST_H
#define BST_H

#include "Stock.h"
#include <iostream>
#include <iomanip>

using namespace std;

// ─────────────────────────────────────────────
// BST Node
// ─────────────────────────────────────────────
struct BSTNode {
    Stock   data;
    BSTNode* left;
    BSTNode* right;

    BSTNode(Stock s) : data(s), left(nullptr), right(nullptr) {}
};

// ─────────────────────────────────────────────
// BSTPriceTree — Binary Search Tree keyed by Price
// For demonstrating: insert, search, inorder, preorder, postorder
// ─────────────────────────────────────────────
class BSTPriceTree {
private:
    BSTNode* root;
    int nodeCount;

    // ── Insert recursively ──
    BSTNode* insertRec(BSTNode* node, const Stock& s) {
        if (!node) {
            nodeCount++;
            return new BSTNode(s);
        }
        if (s.price < node->data.price)
            node->left = insertRec(node->left, s);
        else if (s.price > node->data.price)
            node->right = insertRec(node->right, s);
        else {
            // Same price — insert to right
            node->right = insertRec(node->right, s);
        }
        return node;
    }

    // ── Inorder traversal (ascending price) ──
    void inorderRec(BSTNode* node) const {
        if (!node) return;
        inorderRec(node->left);
        cout << "    ₹" << fixed << setprecision(2) << setw(8) << node->data.price
             << "  │  " << setw(12) << left << node->data.symbol
             << "  │  " << node->data.company << "\n";
        inorderRec(node->right);
    }

    // ── Preorder traversal ──
    void preorderRec(BSTNode* node) const {
        if (!node) return;
        cout << "    " << node->data.symbol
             << " (₹" << fixed << setprecision(2) << node->data.price << ")  ";
        preorderRec(node->left);
        preorderRec(node->right);
    }

    // ── Postorder traversal ──
    void postorderRec(BSTNode* node) const {
        if (!node) return;
        postorderRec(node->left);
        postorderRec(node->right);
        cout << "    " << node->data.symbol
             << " (₹" << fixed << setprecision(2) << node->data.price << ")  ";
    }

    // ── Search by price range ──
    void searchRangeRec(BSTNode* node, double minP, double maxP) const {
        if (!node) return;
        if (minP < node->data.price)
            searchRangeRec(node->left, minP, maxP);
        if (node->data.price >= minP && node->data.price <= maxP) {
            cout << "    ✓ " << setw(12) << left << node->data.symbol
                 << " — ₹" << fixed << setprecision(2) << node->data.price << "\n";
        }
        if (maxP > node->data.price)
            searchRangeRec(node->right, minP, maxP);
    }

    // ── Find minimum ──
    BSTNode* findMin(BSTNode* node) const {
        while (node && node->left)
            node = node->left;
        return node;
    }

    // ── Find maximum ──
    BSTNode* findMax(BSTNode* node) const {
        while (node && node->right)
            node = node->right;
        return node;
    }

    // ── Delete node ──
    BSTNode* deleteRec(BSTNode* node, double price) {
        if (!node) return nullptr;
        if (price < node->data.price)
            node->left = deleteRec(node->left, price);
        else if (price > node->data.price)
            node->right = deleteRec(node->right, price);
        else {
            // Node to delete found
            if (!node->left) {
                BSTNode* temp = node->right;
                delete node;
                nodeCount--;
                return temp;
            } else if (!node->right) {
                BSTNode* temp = node->left;
                delete node;
                nodeCount--;
                return temp;
            }
            // Two children — replace with inorder successor
            BSTNode* succ = findMin(node->right);
            node->data  = succ->data;
            node->right = deleteRec(node->right, succ->data.price);
        }
        return node;
    }

    // ── Height ──
    int heightRec(BSTNode* node) const {
        if (!node) return -1;
        int lh = heightRec(node->left);
        int rh = heightRec(node->right);
        return 1 + max(lh, rh);
    }

    // ── Cleanup ──
    void destroyRec(BSTNode* node) {
        if (!node) return;
        destroyRec(node->left);
        destroyRec(node->right);
        delete node;
    }

    // ── Print tree structure (visual) ──
    void printTreeRec(BSTNode* node, string prefix, bool isLeft) const {
        if (!node) return;
        cout << prefix;
        cout << (isLeft ? "├── " : "└── ");
        cout << node->data.symbol << " (₹"
             << fixed << setprecision(0) << node->data.price << ")\n";
        printTreeRec(node->left,  prefix + (isLeft ? "│   " : "    "), true);
        printTreeRec(node->right, prefix + (isLeft ? "│   " : "    "), false);
    }

public:
    BSTPriceTree() : root(nullptr), nodeCount(0) {}
    ~BSTPriceTree() { destroyRec(root); }

    // Public interface
    void insert(const Stock& s)    { root = insertRec(root, s); }
    void remove(double price)      { root = deleteRec(root, price); }
    int  height() const            { return heightRec(root); }
    int  size()   const            { return nodeCount; }

    // Traversals
    void inorder() const {
        cout << "\n  ┌──── BST INORDER (Ascending Price) ─────────────┐\n";
        cout << "    " << setw(10) << right << "Price" << "  │  "
             << setw(12) << left << "Symbol" << "  │  Company\n";
        cout << "    ──────────────────────────────────────────────────\n";
        inorderRec(root);
        cout << "  └───────────────────────────────────────────────────┘\n";
    }

    void preorder() const {
        cout << "\n  ┌──── BST PREORDER (Root → Left → Right) ────────┐\n  ";
        preorderRec(root);
        cout << "\n  └────────────────────────────────────────────────┘\n";
    }

    void postorder() const {
        cout << "\n  ┌──── BST POSTORDER (Left → Right → Root) ───────┐\n  ";
        postorderRec(root);
        cout << "\n  └────────────────────────────────────────────────┘\n";
    }

    // Search
    void searchRange(double minP, double maxP) const {
        cout << "\n  BST Range Search: ₹" << minP << " — ₹" << maxP << "\n";
        cout << "  ─────────────────────────────────────────────\n";
        searchRangeRec(root, minP, maxP);
    }

    // Cheapest / Costliest
    void showCheapest() const {
        BSTNode* m = findMin(root);
        if (m) {
            cout << "  💰 Cheapest Stock: " << m->data.symbol
                 << " @ ₹" << fixed << setprecision(2) << m->data.price << "\n";
        }
    }

    void showCostliest() const {
        BSTNode* m = findMax(root);
        if (m) {
            cout << "  💎 Costliest Stock: " << m->data.symbol
                 << " @ ₹" << fixed << setprecision(2) << m->data.price << "\n";
        }
    }

    // Visual tree print
    void printTree() const {
        cout << "\n  ┌──── BST STRUCTURE (Visual) ──────────────────┐\n";
        if (root) {
            printTreeRec(root, "    ", false);
        } else {
            cout << "    (empty tree)\n";
        }
        cout << "  └───────────────────────────────────────────────┘\n";
        cout << "  Height: " << height() << "  |  Nodes: " << nodeCount << "\n";
    }
};

#endif // BST_H
