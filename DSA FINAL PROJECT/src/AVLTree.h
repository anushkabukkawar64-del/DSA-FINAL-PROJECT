// ============================================================
// AVLTree.h - Self-Balancing Binary Search Tree for Portfolio
// Smart Stock Market Platform - Indian Stock Market
// B.Tech CSE 2025-29 | DSA with C++ | Sem II | ITM Skills University
// ============================================================

#ifndef AVLTREE_H
#define AVLTREE_H

#include <string>
#include <iostream>
#include <iomanip>
#include <algorithm>

using namespace std;

// ─────────────────────────────────────────────
// Portfolio Item structure
// ─────────────────────────────────────────────
struct PortfolioItem {
    string symbol;
    int quantity;
    double avgPrice;       // Average purchase price in ₹
    double currentPrice;   // Current market price in ₹

    PortfolioItem() : symbol(""), quantity(0), avgPrice(0.0), currentPrice(0.0) {}
    PortfolioItem(string sym, int qty, double avg, double curr)
        : symbol(sym), quantity(qty), avgPrice(avg), currentPrice(curr) {}

    double getInvestment() const { return quantity * avgPrice; }
    double getCurrentValue() const { return quantity * currentPrice; }
    double getPL() const { return getCurrentValue() - getInvestment(); }
    double getPLPct() const {
        double inv = getInvestment();
        if (inv == 0) return 0.0;
        return (getPL() / inv) * 100.0;
    }
};

// ─────────────────────────────────────────────
// AVL Tree Node
// ─────────────────────────────────────────────
struct AVLNode {
    PortfolioItem data;
    AVLNode* left;
    AVLNode* right;
    int height;

    AVLNode(PortfolioItem item) : data(item), left(nullptr), right(nullptr), height(1) {}
};

// ─────────────────────────────────────────────
// AVLPortfolio — Balanced BST of holdings
// Keyed by Stock Symbol (alphabetical)
// ─────────────────────────────────────────────
class AVLPortfolio {
private:
    AVLNode* root;

    // Helper functions
    int getHeight(AVLNode* node) const {
        return node ? node->height : 0;
    }

    int getBalanceFactor(AVLNode* node) const {
        return node ? getHeight(node->left) - getHeight(node->right) : 0;
    }

    void updateHeight(AVLNode* node) {
        if (node) {
            node->height = 1 + max(getHeight(node->left), getHeight(node->right));
        }
    }

    // Rotations
    AVLNode* rotateRight(AVLNode* y) {
        AVLNode* x = y->left;
        AVLNode* T2 = x->right;

        // Perform rotation
        x->right = y;
        y->left = T2;

        // Update heights
        updateHeight(y);
        updateHeight(x);

        return x; // New root
    }

    AVLNode* rotateLeft(AVLNode* x) {
        AVLNode* y = x->right;
        AVLNode* T2 = y->left;

        // Perform rotation
        y->left = x;
        x->right = T2;

        // Update heights
        updateHeight(x);
        updateHeight(y);

        return y; // New root
    }

    // Insert recursively
    AVLNode* insertRec(AVLNode* node, const PortfolioItem& item, bool& updated) {
        if (!node) {
            return new AVLNode(item);
        }

        if (item.symbol < node->data.symbol) {
            node->left = insertRec(node->left, item, updated);
        } else if (item.symbol > node->data.symbol) {
            node->right = insertRec(node->right, item, updated);
        } else {
            // Already exists - update holding (average out price)
            double prevCost = node->data.getInvestment();
            double newCost = item.quantity * item.avgPrice;
            int totalQty = node->data.quantity + item.quantity;
            
            if (totalQty > 0) {
                node->data.avgPrice = (prevCost + newCost) / totalQty;
                node->data.quantity = totalQty;
            }
            node->data.currentPrice = item.currentPrice;
            updated = true;
            return node;
        }

        updateHeight(node);

        int balance = getBalanceFactor(node);

        // LL Case
        if (balance > 1 && item.symbol < node->left->data.symbol)
            return rotateRight(node);

        // RR Case
        if (balance < -1 && item.symbol > node->right->data.symbol)
            return rotateLeft(node);

        // LR Case
        if (balance > 1 && item.symbol > node->left->data.symbol) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }

        // RL Case
        if (balance < -1 && item.symbol < node->right->data.symbol) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }

        return node;
    }

    AVLNode* findMin(AVLNode* node) const {
        AVLNode* current = node;
        while (current && current->left)
            current = current->left;
        return current;
    }

    // Delete recursively
    AVLNode* deleteRec(AVLNode* node, const string& symbol, bool& deleted) {
        if (!node) return nullptr;

        if (symbol < node->data.symbol) {
            node->left = deleteRec(node->left, symbol, deleted);
        } else if (symbol > node->data.symbol) {
            node->right = deleteRec(node->right, symbol, deleted);
        } else {
            // Found node to delete
            deleted = true;
            if (!node->left || !node->right) {
                AVLNode* temp = node->left ? node->left : node->right;
                if (!temp) {
                    temp = node;
                    node = nullptr;
                } else {
                    *node = *temp; // Copy contents
                }
                delete temp;
            } else {
                AVLNode* temp = findMin(node->right);
                node->data = temp->data;
                node->right = deleteRec(node->right, temp->data.symbol, deleted);
            }
        }

        if (!node) return nullptr;

        updateHeight(node);

        int balance = getBalanceFactor(node);

        // LL
        if (balance > 1 && getBalanceFactor(node->left) >= 0)
            return rotateRight(node);

        // LR
        if (balance > 1 && getBalanceFactor(node->left) < 0) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }

        // RR
        if (balance < -1 && getBalanceFactor(node->right) <= 0)
            return rotateLeft(node);

        // RL
        if (balance < -1 && getBalanceFactor(node->right) > 0) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }

        return node;
    }

    // Find node
    AVLNode* findRec(AVLNode* node, const string& symbol) const {
        if (!node || node->data.symbol == symbol)
            return node;
        if (symbol < node->data.symbol)
            return findRec(node->left, symbol);
        return findRec(node->right, symbol);
    }

    // Inorder traversal to print portfolio
    void printInorder(AVLNode* node, double& totalInv, double& totalVal) const {
        if (!node) return;
        printInorder(node->left, totalInv, totalVal);

        PortfolioItem item = node->data;
        totalInv += item.getInvestment();
        totalVal += item.getCurrentValue();

        string sign = (item.getPL() >= 0) ? "+" : "";
        string arrow = (item.getPL() >= 0) ? "▲" : "▼";

        cout << "  │ " << left << setw(11) << item.symbol
             << setw(6) << item.quantity
             << "₹" << setw(11) << fixed << setprecision(2) << item.avgPrice
             << "₹" << setw(11) << item.currentPrice
             << "₹" << setw(11) << item.getInvestment()
             << "₹" << setw(11) << item.getCurrentValue()
             << setw(2) << arrow << sign << fixed << setprecision(2) << item.getPL() 
             << " (" << sign << fixed << setprecision(1) << item.getPLPct() << "%)"
             << "\n";

        printInorder(node->right, totalInv, totalVal);
    }

    // Flatten to list for calculations
    void collectItems(AVLNode* node, PortfolioItem items[], int& count) const {
        if (!node) return;
        collectItems(node->left, items, count);
        items[count++] = node->data;
        collectItems(node->right, items, count);
    }

    void destroyRec(AVLNode* node) {
        if (!node) return;
        destroyRec(node->left);
        destroyRec(node->right);
        delete node;
    }

    // Print AVL visualization
    void printAVLRec(AVLNode* node, string prefix, bool isLeft) const {
        if (!node) return;
        cout << prefix;
        cout << (isLeft ? "├── " : "└── ");
        cout << node->data.symbol << " [H:" << node->height << ", Qty:" << node->data.quantity << "]\n";
        printAVLRec(node->left,  prefix + (isLeft ? "│   " : "    "), true);
        printAVLRec(node->right, prefix + (isLeft ? "│   " : "    "), false);
    }

public:
    AVLPortfolio() : root(nullptr) {}
    ~AVLPortfolio() { destroyRec(root); }

    // Insert or update holding
    void insert(const PortfolioItem& item) {
        bool updated = false;
        root = insertRec(root, item, updated);
    }

    // Sell stock: reduce quantity or delete node if quantity goes to 0
    bool sell(const string& symbol, int quantity, PortfolioItem& removedDetails) {
        AVLNode* node = findRec(root, symbol);
        if (!node) return false;

        if (node->data.quantity < quantity) {
            return false; // Not enough quantity to sell
        }

        removedDetails = node->data; // return state before sell
        removedDetails.quantity = quantity; // we only sold this much

        if (node->data.quantity == quantity) {
            bool deleted = false;
            root = deleteRec(root, symbol, deleted);
        } else {
            node->data.quantity -= quantity;
        }
        return true;
    }

    // Direct search
    PortfolioItem* search(const string& symbol) const {
        AVLNode* res = findRec(root, symbol);
        return res ? &(res->data) : nullptr;
    }

    // Update current price in portfolio when market updates
    void updateCurrentPrice(const string& symbol, double currentPrice) {
        AVLNode* res = findRec(root, symbol);
        if (res) {
            res->data.currentPrice = currentPrice;
        }
    }

    // Print full portfolio holdings table
    void printPortfolio(double& grandTotalInvestment, double& grandTotalCurrentValue) const {
        grandTotalInvestment = 0;
        grandTotalCurrentValue = 0;

        if (!root) {
            cout << "\n  💼 Portfolio is empty! Start buying Indian stocks (e.g. RELIANCE, TCS).\n";
            return;
        }

        cout << "\n  ┌────────────────────────────── CURRENT HOLDINGS (AVL Portfolio Tree) ──────────────────────────────┐\n";
        cout << "  │ Symbol     Qty   Avg Price   Mkt Price   Invested    Current Val  P&L Status (INR ₹)               │\n";
        cout << "  ├────────────────────────────────────────────────────────────────────────────────────────────────────┤\n";
        printInorder(root, grandTotalInvestment, grandTotalCurrentValue);
        cout << "  └────────────────────────────────────────────────────────────────────────────────────────────────────┘\n";

        double netPL = grandTotalCurrentValue - grandTotalInvestment;
        double netPLPct = (grandTotalInvestment > 0) ? (netPL / grandTotalInvestment) * 100.0 : 0.0;
        string sign = (netPL >= 0) ? "+" : "";
        string icon = (netPL >= 0) ? "📈" : "📉";

        cout << fixed << setprecision(2);
        cout << "  💰 Total Investment   : ₹" << grandTotalInvestment << "\n";
        cout << "  📊 Current Port. Value: ₹" << grandTotalCurrentValue << "\n";
        cout << "  " << icon << " Net Profit / Loss   : " << sign << "₹" << netPL << " (" << sign << setprecision(1) << netPLPct << "%)\n";
    }

    // Get count of holdings
    int getHoldingsCount() const {
        int count = 0;
        countNodes(root, count);
        return count;
    }

    void countNodes(AVLNode* node, int& count) const {
        if (!node) return;
        count++;
        countNodes(node->left, count);
        countNodes(node->right, count);
    }

    // Return list of items for dashboard metrics
    void getHoldingsList(PortfolioItem items[], int& count) const {
        count = 0;
        collectItems(root, items, count);
    }

    // Visual tree structure for verification
    void printTreeStructure() const {
        cout << "\n  ┌──── AVL PORTFOLIO STRUCTURE (Visual Balance Check) ──┐\n";
        if (root) {
            printAVLRec(root, "    ", false);
        } else {
            cout << "    (Portfolio empty)\n";
        }
        cout << "  └──────────────────────────────────────────────────────┘\n";
    }
};

#endif // AVLTREE_H
