// ============================================================
// Dashboard.h - Market & Portfolio Analytics Dashboard
// Smart Stock Market Platform - Indian Stock Market
// B.Tech CSE 2025-29 | DSA with C++ | Sem II | ITM Skills University
// ============================================================

#ifndef DASHBOARD_H
#define DASHBOARD_H

#include "Stock.h"
#include "Portfolio.h"
#include "StockHashMap.h"
#include <iostream>
#include <iomanip>
#include <string>

using namespace std;

// ─────────────────────────────────────────────
// Dashboard class
// Renders live metrics, calculates top stock, trend, etc.
// ─────────────────────────────────────────────
class Dashboard {
public:
    static void displayDashboard(PortfolioManager& pm, StockHashMap& market) {
        // Update portfolio current prices
        pm.updatePortfolioPrices();

        int count = 0;
        Stock* stocks = market.getAllStocks(count);

        // 1. Calculate Market Trend based on average growth % of all stocks
        double totalGrowth = 0.0;
        for (int i = 0; i < count; i++) {
            totalGrowth += stocks[i].growthPct;
        }
        double avgGrowth = (count > 0) ? (totalGrowth / count) : 0.0;
        
        string trendStr = "NEUTRAL ➡️";
        if (avgGrowth > 1.5) {
            trendStr = "STRONGLY BULLISH 🚀";
        } else if (avgGrowth > 0.0) {
            trendStr = "BULLISH 📈";
        } else if (avgGrowth < -1.5) {
            trendStr = "STRONGLY BEARISH 📉";
        } else if (avgGrowth < 0.0) {
            trendStr = "BEARISH 📉";
        }

        // 2. Calculate Top Performing Stock in the Market
        Stock topStock;
        double maxGrowth = -9999.0;
        for (int i = 0; i < count; i++) {
            if (stocks[i].growthPct > maxGrowth) {
                maxGrowth = stocks[i].growthPct;
                topStock = stocks[i];
            }
        }

        // 3. Fetch Portfolio calculations from holdings AVL
        double totalInvestment = 0.0;
        double currentPortfolioValue = 0.0;
        pm.getHoldings().printPortfolio(totalInvestment, currentPortfolioValue);

        double netPL = currentPortfolioValue - totalInvestment;
        double plPct = (totalInvestment > 0) ? (netPL / totalInvestment) * 100.0 : 0.0;
        string plSign = (netPL >= 0) ? "+" : "";

        // 4. Find Top Performing Stock inside the portfolio (if any)
        int holdingsCount = 0;
        PortfolioItem items[50];
        pm.getHoldings().getHoldingsList(items, holdingsCount);

        string topHoldingSymbol = "N/A";
        double maxHoldingPL = -9999.0;
        for (int i = 0; i < holdingsCount; i++) {
            if (items[i].getPL() > maxHoldingPL) {
                maxHoldingPL = items[i].getPL();
                topHoldingSymbol = items[i].symbol;
            }
        }

        // Output UI Render
        cout << "\n";
        cout << "  ╔══════════════════════════════════════════════════════════════════╗\n";
        cout << "  ║          🇮🇳 SMART INDIAN STOCK MARKET ANALYTICS DASHBOARD        ║\n";
        cout << "  ╠══════════════════════════════════════════════════════════════════╣\n";
        cout << fixed << setprecision(2);
        cout << "  ║  Total Investment     : ₹" << left << setw(41) << totalInvestment << "║\n";
        cout << "  ║  Current Portfolio Val: ₹" << left << setw(41) << currentPortfolioValue << "║\n";
        cout << "  ║  Net Portfolio P&L    : " << plSign << "₹" << left << setw(15) << netPL 
             << " (" << plSign << setprecision(1) << plPct << "%)" << string(19, ' ') << "║\n";
        cout << "  ║  Cash Balance         : ₹" << setprecision(2) << left << setw(41) << pm.getCashBalance() << "║\n";
        cout << "  ║  Total Net Asset Value: ₹" << left << setw(41) << (pm.getCashBalance() + currentPortfolioValue) << "║\n";
        cout << "  ├──────────────────────────────────────────────────────────────────┤\n";
        cout << "  ║  Market Trend         : " << left << setw(41) << trendStr << "║\n";
        if (count > 0) {
            string topStockStr = topStock.symbol + " (" + topStock.company + ") +" + to_string((int)topStock.growthPct) + "%";
            cout << "  ║  Top Market Performer : " << left << setw(41) << topStockStr.substr(0, 41) << "║\n";
        }
        cout << "  ║  Top Portfolio Asset  : " << left << setw(41) << (topHoldingSymbol + (topHoldingSymbol != "N/A" ? " (₹" + to_string((int)maxHoldingPL) + ")" : "")) << "║\n";
        cout << "  ╚══════════════════════════════════════════════════════════════════╝\n";
        cout << "\n";

        delete[] stocks;
    }
};

#endif // DASHBOARD_H
