// ============================================================
// Stock.h - Stock Data Model
// Smart Stock Market Platform - Indian Stock Market
// B.Tech CSE 2025-29 | DSA with C++ | Sem II | ITM Skills University
// ============================================================

#ifndef STOCK_H
#define STOCK_H

#include <string>
#include <iostream>
#include <iomanip>

using namespace std;

// ─────────────────────────────────────────────
// Stock Structure — represents one NSE/BSE stock
// ─────────────────────────────────────────────
struct Stock {
    string symbol;       // NSE Symbol e.g. "RELIANCE"
    string company;      // Full company name
    string sector;       // Sector (Energy, Technology, etc.)
    double price;        // Current price in ₹
    double growthPct;    // % change today e.g. +3.2
    double high52w;      // 52-week high ₹
    double low52w;       // 52-week low ₹
    long long volume;    // Today's traded volume (shares)

    Stock() : price(0), growthPct(0), high52w(0), low52w(0), volume(0) {}

    Stock(string sym, string comp, string sec,
          double pr, double growth, double h52, double l52, long long vol)
        : symbol(sym), company(comp), sector(sec),
          price(pr), growthPct(growth), high52w(h52), low52w(l52), volume(vol) {}

    // Print a formatted one-line summary
    void printSummary() const {
        string trend = (growthPct >= 0) ? "▲" : "▼";
        string sign  = (growthPct >= 0) ? "+" : "";
        cout << left
             << setw(12) << symbol
             << setw(30) << company
             << setw(16) << sector
             << "₹" << right << setw(8) << fixed << setprecision(2) << price
             << "  " << trend << " " << sign
             << fixed << setprecision(2) << growthPct << "%"
             << "\n";
    }

    // Print detailed card
    void printDetail() const {
        string sign = (growthPct >= 0) ? "+" : "";
        cout << "\n┌─────────────────────────────────────────────┐\n";
        cout << "│  " << symbol << " — " << company << "\n";
        cout << "│  Sector   : " << sector << "\n";
        cout << fixed << setprecision(2);
        cout << "│  Price    : ₹" << price << "\n";
        cout << "│  Change   : " << sign << growthPct << "%\n";
        cout << "│  52W High : ₹" << high52w << "   Low : ₹" << low52w << "\n";
        cout << "│  Volume   : " << volume << " shares\n";
        cout << "└─────────────────────────────────────────────┘\n";
    }
};

// ─────────────────────────────────────────────
// Transaction — used by Stack (Undo) and Queue (Alert)
// ─────────────────────────────────────────────
struct Transaction {
    string symbol;
    int    quantity;
    double price;
    string type;   // "BUY" or "SELL"

    Transaction() : quantity(0), price(0) {}
    Transaction(string sym, int qty, double pr, string t)
        : symbol(sym), quantity(qty), price(pr), type(t) {}
};

// ─────────────────────────────────────────────
// MarketAlert — used by Queue
// ─────────────────────────────────────────────
struct MarketAlert {
    string symbol;
    string message;
    string alertType;  // "PRICE_SPIKE", "VOLUME_SURGE", "NEWS"
    double value;

    MarketAlert() : value(0) {}
    MarketAlert(string sym, string msg, string atype, double val)
        : symbol(sym), message(msg), alertType(atype), value(val) {}
};

#endif // STOCK_H
