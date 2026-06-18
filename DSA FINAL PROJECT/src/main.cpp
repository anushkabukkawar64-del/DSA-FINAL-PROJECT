// ============================================================
// main.cpp - Entry Point and User Menu
// Smart Stock Market Platform - Indian Stock Market
// B.Tech CSE 2025-29 | DSA with C++ | Sem II | ITM Skills University
// ============================================================

#include "Stock.h"
#include "StockHashMap.h"
#include "Sorting.h"
#include "Searching.h"
#include "Stack.h"
#include "Queue.h"
#include "BST.h"
#include "AVLTree.h"
#include "Graph.h"
#include "Portfolio.h"
#include "Dashboard.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <cstdlib>
#include <ctime>

using namespace std;

// ─────────────────────────────────────────────
// Global variables
// ─────────────────────────────────────────────
const int NUM_SAMPLE_STOCKS = 10;
Stock sampleStocks[NUM_SAMPLE_STOCKS] = {
    Stock("RELIANCE",   "Reliance Industries",      "Energy",         1450.0,  3.2,  1620.0, 1280.0, 4500000),
    Stock("TCS",        "Tata Consultancy Services","Technology",     3850.0,  2.5,  4200.0, 3400.0, 1800000),
    Stock("INFY",       "Infosys",                  "Technology",     1620.0,  1.8,  1750.0, 1350.0, 2900000),
    Stock("HDFCBANK",   "HDFC Bank",                "Banking",        1680.0,  2.9,  1800.0, 1420.0, 5200000),
    Stock("ICICIBANK",  "ICICI Bank",               "Banking",        1240.0,  3.5,  1320.0, 1050.0, 3800000),
    Stock("SBIN",       "State Bank of India",      "Banking",         920.0,  4.1,   980.0,  710.0, 6100000),
    Stock("BHARTIARTL", "Bharti Airtel",            "Telecom",        1870.0,  2.1,  1950.0, 1490.0, 2200000),
    Stock("ITC",        "ITC Limited",              "FMCG",            510.0,  1.6,   540.0,  420.0, 8500000),
    Stock("LT",         "Larsen & Toubro",          "Infrastructure", 3620.0,  1.9,  3900.0, 3100.0, 1200000),
    Stock("ADANIENT",   "Adani Enterprises",        "Energy",         2850.0,  5.3,  3200.0, 2400.0, 3100000)
};

StockHashMap marketMap;
BSTPriceTree bstPrice;
SectorGraph sectorGraph;
AlertQueue alertQueue;
PortfolioManager* portfolio = nullptr;

// ─────────────────────────────────────────────
// Function Prototypes
// ─────────────────────────────────────────────
void initMarket();
void simulateTickerTick();
void displayMainMenu();
void handleMarketMenu();
void handleSearchMenu();
void handlePortfolioMenu();
void handleGraphMenu();
void handleAlertMenu();
void handleAcademicDemoMenu();

// Helper functions
void pressAnyKey();
string toUpper(const string& s);

// ─────────────────────────────────────────────
// Entry point
// ─────────────────────────────────────────────
int main() {
    // Seed random generator for ticker simulation
    srand(static_cast<unsigned>(time(nullptr)));

    // Initialize data structures
    initMarket();

    // Start portfolio with initial cash of ₹10,00,000 (10 Lakhs)
    portfolio = new PortfolioManager(1000000.0, alertQueue, marketMap);

    // Initial alert queue generation
    alertQueue.generateAlerts(sampleStocks, NUM_SAMPLE_STOCKS);

    int choice = 0;
    do {
        displayMainMenu();
        cout << "  Enter choice [1-8]: ";
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "  ⚠ Invalid Input. Try again.\n";
            continue;
        }

        switch (choice) {
            case 1:
                Dashboard::displayDashboard(*portfolio, marketMap);
                pressAnyKey();
                break;
            case 2:
                handleMarketMenu();
                break;
            case 3:
                handleSearchMenu();
                break;
            case 4:
                handlePortfolioMenu();
                break;
            case 5:
                handleGraphMenu();
                break;
            case 6:
                handleAlertMenu();
                break;
            case 7:
                handleAcademicDemoMenu();
                break;
            case 8:
                cout << "\n  Thank you for using the Smart Stock Market Platform!\n  Exiting...\n\n";
                break;
            default:
                cout << "  ⚠ Invalid choice. Enter 1-8.\n";
                pressAnyKey();
                break;
        }
    } while (choice != 8);

    delete portfolio;
    return 0;
}

// ─────────────────────────────────────────────
// Initialize Market structures
// ─────────────────────────────────────────────
void initMarket() {
    // 1. Populate O(1) Hash Map
    for (int i = 0; i < NUM_SAMPLE_STOCKS; i++) {
        marketMap.insert(sampleStocks[i].symbol, sampleStocks[i]);
    }

    // 2. Populate BST (Price-based sorting)
    for (int i = 0; i < NUM_SAMPLE_STOCKS; i++) {
        bstPrice.insert(sampleStocks[i]);
    }

    // 3. Setup sector relationships
    sectorGraph.loadDefaultRelationships();
}

// ─────────────────────────────────────────────
// Simulate Market Ticker updates
// Randomly fluctuates prices by -3% to +4%
// ─────────────────────────────────────────────
void simulateTickerTick() {
    cout << "\n  ⏳ Simulating Live market fluctuations (INR ₹) and feed triggers...\n";
    int count = 0;
    Stock* arr = marketMap.getAllStocks(count);
    
    for (int i = 0; i < count; i++) {
        double pctChange = ((rand() % 70) - 30) / 10.0; // -3.0% to +4.0%
        double prevPrice = arr[i].price;
        double newPrice = prevPrice * (1.0 + pctChange / 100.0);
        
        arr[i].price = newPrice;
        arr[i].growthPct = pctChange;

        // Update 52-week highs/lows
        if (newPrice > arr[i].high52w) arr[i].high52w = newPrice;
        if (newPrice < arr[i].low52w)  arr[i].low52w = newPrice;

        // Update volume slightly
        arr[i].volume += (rand() % 500000);

        // Save back to market HashMap
        marketMap.insert(arr[i].symbol, arr[i]);
    }

    // Update prices in Portfolio if matching stocks exist
    portfolio->updatePortfolioPrices();

    // Regenerate BST with updated prices
    // Destructor of BSTPriceTree cleans up node, so we reset
    bstPrice = BSTPriceTree();
    for (int i = 0; i < count; i++) {
        bstPrice.insert(arr[i]);
    }

    // Trigger queue alerts
    alertQueue.generateAlerts(arr, count);
    
    delete[] arr;
    cout << "  ✓ Market feeds successfully updated with live NSE updates!\n";
}

// ─────────────────────────────────────────────
// Navigation & Views
// ─────────────────────────────────────────────
void displayMainMenu() {
    cout << "\n";
    cout << "  ==============================================================\n";
    cout << "  🟢 SMART STOCK ANALYSIS & PORTFOLIO MANAGEMENT PLATFORM (NSE/BSE)\n";
    cout << "  ==============================================================\n";
    cout << "  1. Live Investment Dashboard (Live Metrics)\n";
    cout << "  2. Market Stocks Directory (Sort / List / Live Updates)\n";
    cout << "  3. Search Engine (Symbol / Company / Price Range)\n";
    cout << "  4. Portfolio Holdings Terminal (Buy / Sell / Revert Transaction)\n";
    cout << "  5. Sector Correlation & Graph Explorer (BFS / DFS Relationships)\n";
    cout << "  6. Market Alert Center (Queue Alerts)\n";
    cout << "  7. Academic DSA Demonstration (BST/AVL/Hash Diagnostics)\n";
    cout << "  8. Exit System\n";
    cout << "  ==============================================================\n";
}

void handleMarketMenu() {
    int choice = 0;
    int count = 0;
    Stock* arr = marketMap.getAllStocks(count);

    cout << "\n  --- MARKET STOCKS DIRECTORY --- \n";
    cout << "  1. View Current Directory (Default Hash Order)\n";
    cout << "  2. Sort by Stock Price (Bubble Sort)\n";
    cout << "  3. Sort by Market Performance/Gain % (Selection Sort)\n";
    cout << "  4. Sort Alphabetically by Symbol (Merge Sort)\n";
    cout << "  5. Sort by Daily Volume (Quick Sort)\n";
    cout << "  6. Trigger Market Ticker Tick (Simulate Live Price Update)\n";
    cout << "  7. Back to Main Menu\n";
    cout << "  Enter choice: ";
    cin >> choice;

    switch (choice) {
        case 1:
            cout << "\n";
            Sorting::displaySorted(arr, count, "Default Unsorted Hash Feed");
            break;
        case 2:
            Sorting::bubbleSortByPrice(arr, count, true);
            Sorting::displaySorted(arr, count, "Price Ascending (Bubble Sort)");
            break;
        case 3:
            Sorting::selectionSortByGain(arr, count, false); // highest gain first
            Sorting::displaySorted(arr, count, "Gain % Descending (Selection Sort)");
            break;
        case 4:
            Sorting::mergeSortBySymbol(arr, 0, count - 1);
            Sorting::displaySorted(arr, count, "Alphabetical by Symbol (Merge Sort)");
            break;
        case 5:
            Sorting::quickSortByVolume(arr, 0, count - 1);
            Sorting::displaySorted(arr, count, "Daily Traded Volume (Quick Sort)");
            break;
        case 6:
            simulateTickerTick();
            break;
        case 7:
            break;
        default:
            cout << "  ⚠ Invalid choice.\n";
    }
    delete[] arr;
    pressAnyKey();
}

void handleSearchMenu() {
    int choice = 0;
    int count = 0;
    Stock* arr = marketMap.getAllStocks(count);

    cout << "\n  --- STOCK SEARCH ENGINE --- \n";
    cout << "  1. Search exact Symbol (Binary Search - O(log N))\n";
    cout << "  2. Search by Company Name or Symbol keyword (Linear Search - O(N))\n";
    cout << "  3. Search Stocks by Sector (Linear Search)\n";
    cout << "  4. Search by price ranges (BST Range Search)\n";
    cout << "  5. Back\n";
    cout << "  Enter choice: ";
    cin >> choice;

    if (choice == 1) {
        string sym;
        cout << "  Enter symbol (e.g. INFY, SBIN): ";
        cin >> sym;
        // Binary search requires array sorted by Symbol
        Sorting::mergeSortBySymbol(arr, 0, count - 1);
        int idx = Searching::binarySearchBySymbol(arr, count, sym);
        Searching::displayResult(arr, idx, sym);
    } 
    else if (choice == 2) {
        string query;
        cout << "  Enter company name keyword (e.g. Reliance, Tata): ";
        cin.ignore();
        getline(cin, query);
        int idx = Searching::linearSearchByName(arr, count, query);
        Searching::displayResult(arr, idx, query);
    } 
    else if (choice == 3) {
        string sector;
        cout << "  Enter Sector (Banking, Technology, Energy, FMCG, Telecom, Infrastructure): ";
        cin.ignore();
        getline(cin, sector);
        Searching::linearSearchBySector(arr, count, sector);
    } 
    else if (choice == 4) {
        double minPrice, maxPrice;
        cout << "  Enter Minimum Price (₹): ";
        cin >> minPrice;
        cout << "  Enter Maximum Price (₹): ";
        cin >> maxPrice;
        bstPrice.searchRange(minPrice, maxPrice);
    }

    delete[] arr;
    pressAnyKey();
}

void handlePortfolioMenu() {
    int choice = 0;
    cout << "\n  --- PORTFOLIO HOLDINGS TERMINAL --- \n";
    cout << "  1. View Current Holdings & Cash Balance\n";
    cout << "  2. Buy Shares of an Indian Stock (e.g. RELIANCE)\n";
    cout << "  3. Sell Shares of an Indian Stock\n";
    cout << "  4. Revert/Undo Last Transaction (Stack pop - O(1))\n";
    cout << "  5. Back\n";
    cout << "  Enter choice: ";
    cin >> choice;

    if (choice == 1) {
        portfolio->viewPortfolio();
    } 
    else if (choice == 2) {
        string sym;
        int qty;
        cout << "  Enter Stock Symbol to Buy: ";
        cin >> sym;
        cout << "  Enter quantity to purchase: ";
        cin >> qty;
        portfolio->buyStock(toUpper(sym), qty);
    } 
    else if (choice == 3) {
        string sym;
        int qty;
        cout << "  Enter Stock Symbol to Sell: ";
        cin >> sym;
        cout << "  Enter quantity to sell: ";
        cin >> qty;
        portfolio->sellStock(toUpper(sym), qty);
    } 
    else if (choice == 4) {
        portfolio->undoLastTransaction();
    }

    pressAnyKey();
}

void handleGraphMenu() {
    int choice = 0;
    cout << "\n  --- SECTOR RELATIONSHIP EXPLORER --- \n";
    cout << "  1. Print Complete Sector Relationship Adjacency List\n";
    cout << "  2. BFS propagation search (Broad market impact check)\n";
    cout << "  3. DFS correlation search (Risk chain assessment)\n";
    cout << "  4. Get diversification advice based on holding sector\n";
    cout << "  5. Back\n";
    cout << "  Enter choice: ";
    cin >> choice;

    if (choice == 1) {
        sectorGraph.displayGraph();
    } 
    else if (choice == 2) {
        string s;
        cout << "  Enter start sector (e.g. Banking, Energy): ";
        cin >> s;
        sectorGraph.BFS(s);
    } 
    else if (choice == 3) {
        string s;
        cout << "  Enter start sector: ";
        cin >> s;
        sectorGraph.DFS(s);
    } 
    else if (choice == 4) {
        string s;
        cout << "  Enter sector you currently hold assets in: ";
        cin >> s;
        sectorGraph.suggestDiversification(s);
    }

    pressAnyKey();
}

void handleAlertMenu() {
    int choice = 0;
    cout << "\n  --- ALERT NOTIFICATION CENTER --- \n";
    cout << "  1. View All Pending Market Alerts (Queue view)\n";
    cout << "  2. Process/Acknowledge Oldest Alert (FIFO dequeue)\n";
    cout << "  3. Clear all alerts\n";
    cout << "  4. Back\n";
    cout << "  Enter choice: ";
    cin >> choice;

    if (choice == 1) {
        alertQueue.displayAll();
    } 
    else if (choice == 2) {
        alertQueue.processNext();
    } 
    else if (choice == 3) {
        while (!alertQueue.isEmpty()) alertQueue.dequeue();
        cout << "  ✓ Cleared all market notifications.\n";
    }

    pressAnyKey();
}

void handleAcademicDemoMenu() {
    int choice = 0;
    cout << "\n  --- ACADEMIC DSA DIAGNOSTIC TERMINAL --- \n";
    cout << "  1. View BST Structure (Prices Visual tree)\n";
    cout << "  2. BST Traversal Algorithms (Inorder, Preorder, Postorder)\n";
    cout << "  3. View AVL Portfolio balanced structure (Holdings tree)\n";
    cout << "  4. View Stock symbol Hash Map collisions (Chaining details)\n";
    cout << "  5. View Undo Stack details (Linked List history)\n";
    cout << "  6. Back\n";
    cout << "  Enter choice: ";
    cin >> choice;

    if (choice == 1) {
        bstPrice.printTree();
        bstPrice.showCheapest();
        bstPrice.showCostliest();
    } 
    else if (choice == 2) {
        bstPrice.inorder();
        bstPrice.preorder();
        bstPrice.postorder();
    } 
    else if (choice == 3) {
        portfolio->getHoldings().printTreeStructure();
    } 
    else if (choice == 4) {
        marketMap.displayTable();
    } 
    else if (choice == 5) {
        portfolio->getUndoStack().display();
    }

    pressAnyKey();
}

// ─────────────────────────────────────────────
// Auxiliary Helpers
// ─────────────────────────────────────────────
void pressAnyKey() {
    cout << "\n  Press Enter to continue...";
    cin.ignore(10000, '\n');
    cin.get();
}

string toUpper(const string& s) {
    string res = s;
    for (char& c : res) c = toupper(c);
    return res;
}
