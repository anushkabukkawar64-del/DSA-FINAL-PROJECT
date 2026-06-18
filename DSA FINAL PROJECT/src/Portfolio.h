// ============================================================
// Portfolio.h - Portfolio Manager
// Smart Stock Market Platform - Indian Stock Market
// B.Tech CSE 2025-29 | DSA with C++ | Sem II | ITM Skills University
// ============================================================

#ifndef PORTFOLIO_H
#define PORTFOLIO_H

#include "Stock.h"
#include "AVLTree.h"
#include "Stack.h"
#include "Queue.h"
#include "StockHashMap.h"
#include <iostream>
#include <iomanip>
#include <string>

using namespace std;

// ─────────────────────────────────────────────
// PortfolioManager
// Manages Cash, Holdings, Transactions (Undo), Alerts
// ─────────────────────────────────────────────
class PortfolioManager {
private:
    double cashBalance;
    AVLPortfolio holdings;
    TransactionStack undoStack;
    AlertQueue& alertQueue;
    StockHashMap& marketStocks;

public:
    PortfolioManager(double initialCash, AlertQueue& aq, StockHashMap& ms)
        : cashBalance(initialCash), alertQueue(aq), marketStocks(ms) {}

    double getCashBalance() const { return cashBalance; }

    // Buy Stock Operation
    bool buyStock(const string& symbol, int qty) {
        Stock* stock = marketStocks.search(symbol);
        if (!stock) {
            cout << "  ✗ Error: Stock symbol \"" << symbol << "\" not found in Indian Stock Market.\n";
            return false;
        }

        if (qty <= 0) {
            cout << "  ✗ Error: Invalid quantity: " << qty << "\n";
            return false;
        }

        double totalCost = qty * stock->price;
        if (totalCost > cashBalance) {
            cout << fixed << setprecision(2);
            cout << "  ✗ Error: Insufficient funds!\n";
            cout << "    Required : ₹" << totalCost << "\n";
            cout << "    Available: ₹" << cashBalance << "\n";
            return false;
        }

        // Deduct cash
        cashBalance -= totalCost;

        // Insert into AVL Tree
        PortfolioItem item(symbol, qty, stock->price, stock->price);
        holdings.insert(item);

        // Record on Undo Stack
        undoStack.push(Transaction(symbol, qty, stock->price, "BUY"));

        // Trigger an alert
        alertQueue.enqueue(MarketAlert(
            symbol,
            "Successfully BOUGHT " + to_string(qty) + " shares of " + stock->company + " @ ₹" + to_string((int)stock->price),
            "TRANSACTION_ALERT",
            totalCost
        ));

        cout << "\n  ✓ Successfully bought " << qty << " shares of " << symbol << " (" << stock->company << ") @ ₹" << stock->price << " each.\n";
        return true;
    }

    // Sell Stock Operation
    bool sellStock(const string& symbol, int qty) {
        Stock* stock = marketStocks.search(symbol);
        if (!stock) {
            cout << "  ✗ Error: Stock symbol \"" << symbol << "\" not found.\n";
            return false;
        }

        if (qty <= 0) {
            cout << "  ✗ Error: Invalid quantity: " << qty << "\n";
            return false;
        }

        PortfolioItem* item = holdings.search(symbol);
        if (!item) {
            cout << "  ✗ Error: You do not own any shares of " << symbol << ".\n";
            return false;
        }

        if (item->quantity < qty) {
            cout << "  ✗ Error: Insufficient shares in portfolio!\n";
            cout << "    Owned: " << item->quantity << " | Attempted to sell: " << qty << "\n";
            return false;
        }

        // Perform sell in AVL Tree
        PortfolioItem soldDetails;
        if (!holdings.sell(symbol, qty, soldDetails)) {
            cout << "  ✗ Error: Internal Sell Operation failed.\n";
            return false;
        }

        // Add to cash balance
        double payout = qty * stock->price;
        cashBalance += payout;

        // Push transaction to Undo Stack
        undoStack.push(Transaction(symbol, qty, soldDetails.avgPrice, "SELL"));

        // Trigger Alert
        double pl = qty * (stock->price - soldDetails.avgPrice);
        string plSign = (pl >= 0) ? "Profit" : "Loss";
        alertQueue.enqueue(MarketAlert(
            symbol,
            "Successfully SOLD " + to_string(qty) + " shares of " + stock->company + ". " + plSign + ": ₹" + to_string((int)abs(pl)),
            "TRANSACTION_ALERT",
            payout
        ));

        cout << fixed << setprecision(2);
        cout << "\n  ✓ Successfully sold " << qty << " shares of " << symbol << " (" << stock->company << ") @ ₹" << stock->price << " each.\n";
        cout << "    P&L Realized: ₹" << pl << "\n";
        return true;
    }

    // Undo Last Transaction
    bool undoLastTransaction() {
        if (undoStack.isEmpty()) {
            cout << "  ✗ Undo failed: No recent transactions to revert.\n";
            return false;
        }

        Transaction tx = undoStack.pop();
        Stock* stock = marketStocks.search(tx.symbol);

        cout << "\n  🔄 Undoing last transaction: " << tx.type << " " << tx.quantity << " " << tx.symbol << "...\n";

        if (tx.type == "BUY") {
            // Revert BUY: we must remove the stock from holdings and return cash
            PortfolioItem soldDetails;
            if (holdings.sell(tx.symbol, tx.quantity, soldDetails)) {
                // Return purchase cash
                double cost = tx.quantity * tx.price;
                cashBalance += cost;
                cout << "    [REVERTED] Deducted " << tx.quantity << " shares of " << tx.symbol << " from holdings.\n";
                cout << "    [REVERTED] Returned ₹" << cost << " to Cash Balance.\n";
            } else {
                cout << "    ✗ Error: Could not revert BUY. Shares might have been sold already.\n";
                // Restore stack item
                undoStack.push(tx);
                return false;
            }
        } else if (tx.type == "SELL") {
            // Revert SELL: we must buy back the stock at the original average purchase price and deduct cash
            double payout = tx.quantity * (stock ? stock->price : tx.price);
            if (cashBalance < payout) {
                cout << "    ✗ Error: Insufficient cash balance to undo SELL transaction (requires ₹" << payout << ").\n";
                undoStack.push(tx);
                return false;
            }

            cashBalance -= payout;
            
            // Re-insert holding at original purchase price
            PortfolioItem item(tx.symbol, tx.quantity, tx.price, (stock ? stock->price : tx.price));
            holdings.insert(item);

            cout << "    [REVERTED] Restored " << tx.quantity << " shares of " << tx.symbol << " at cost ₹" << tx.price << ".\n";
            cout << "    [REVERTED] Deducted ₹" << payout << " from Cash Balance.\n";
        }

        alertQueue.enqueue(MarketAlert(
            tx.symbol,
            "Reverted last transaction: " + tx.type + " " + to_string(tx.quantity) + " " + tx.symbol,
            "UNDO_ALERT",
            0.0
        ));

        return true;
    }

    // Update prices in portfolio when market updates
    void updatePortfolioPrices() {
        int count = 0;
        Stock* arr = marketStocks.getAllStocks(count);
        for (int i = 0; i < count; i++) {
            holdings.updateCurrentPrice(arr[i].symbol, arr[i].price);
        }
        delete[] arr;
    }

    // View Holdings
    void viewPortfolio() {
        updatePortfolioPrices();
        double totalInv = 0, totalVal = 0;
        holdings.printPortfolio(totalInv, totalVal);
        cout << fixed << setprecision(2);
        cout << "  🏦 Available Cash     : ₹" << cashBalance << "\n";
        cout << "  💼 Net Portfolio Value : ₹" << (cashBalance + totalVal) << "\n";
    }

    // Return the holdings tree reference (for debugging / AVL balance check)
    AVLPortfolio& getHoldings() { return holdings; }
    
    // Return transaction stack reference
    const TransactionStack& getUndoStack() const { return undoStack; }
};

#endif // PORTFOLIO_H
