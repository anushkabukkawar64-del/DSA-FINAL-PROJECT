// ============================================================
// Queue.h - Circular Queue for Market Alerts
// Smart Stock Market Platform - Indian Stock Market
// B.Tech CSE 2025-29 | DSA with C++ | Sem II | ITM Skills University
// ============================================================

#ifndef QUEUE_H
#define QUEUE_H

#include "Stock.h"
#include <iomanip>
#include <iostream>

using namespace std;

// ─────────────────────────────────────────────
// AlertQueue — Circular Queue (Array-based)
// FIFO for market alerts (price spikes, news, etc.)
// ─────────────────────────────────────────────
class AlertQueue {
private:
  static const int MAX_SIZE = 50;
  MarketAlert alerts[MAX_SIZE];
  int front, rear, count;

public:
  AlertQueue() : front(0), rear(-1), count(0) {}

  bool isFull() const { return count == MAX_SIZE; }
  bool isEmpty() const { return count == 0; }
  int size() const { return count; }

  // Enqueue a new alert
  void enqueue(const MarketAlert &alert) {
    if (isFull()) {
      cout << "  [QUEUE] ⚠ Alert queue full! Oldest alert discarded.\n";
      // Overwrite oldest — circular
      front = (front + 1) % MAX_SIZE;
      count--;
    }
    rear = (rear + 1) % MAX_SIZE;
    alerts[rear] = alert;
    count++;

    string icon;
    if (alert.alertType == "PRICE_SPIKE")
      icon = "📈";
    else if (alert.alertType == "PRICE_DROP")
      icon = "📉";
    else if (alert.alertType == "VOLUME_SURGE")
      icon = "🔊";
    else
      icon = "📢";

    cout << "  [QUEUE] " << icon << " Alert queued: " << alert.symbol << " — "
         << alert.message << "\n";
  }

  // Dequeue — process oldest alert
  MarketAlert dequeue() {
    if (isEmpty()) {
      cout << "  [QUEUE] ✗ No pending alerts.\n";
      return MarketAlert();
    }
    MarketAlert alert = alerts[front];
    front = (front + 1) % MAX_SIZE;
    count--;
    return alert;
  }

  // Peek at front
  MarketAlert peek() const {
    if (isEmpty()) {
      cout << "  [QUEUE] ✗ No pending alerts.\n";
      return MarketAlert();
    }
    return alerts[front];
  }

  // Generate sample alerts based on stock data
  void generateAlerts(Stock stockArr[], int n) {
    for (int i = 0; i < n; i++) {
      if (stockArr[i].growthPct >= 4.0) {
        enqueue(MarketAlert(stockArr[i].symbol,
                            stockArr[i].company + " surged +" +
                                to_string((int)stockArr[i].growthPct) +
                                "%! Consider booking profits.",
                            "PRICE_SPIKE", stockArr[i].growthPct));
      }
      if (stockArr[i].growthPct <= -3.0) {
        enqueue(MarketAlert(stockArr[i].symbol,
                            stockArr[i].company + " dropped " +
                                to_string((int)stockArr[i].growthPct) +
                                "%! Buy the dip?",
                            "PRICE_DROP", stockArr[i].growthPct));
      }
      if (stockArr[i].volume > 5000000) {
        enqueue(MarketAlert(stockArr[i].symbol,
                            stockArr[i].company +
                                " — high volume activity detected!",
                            "VOLUME_SURGE", (double)stockArr[i].volume));
      }
    }
  }

  // Display all pending alerts
  void displayAll() const {
    if (isEmpty()) {
      cout << "\n  📭 No pending market alerts.\n";
      return;
    }
    cout << "\n  ┌──────────── MARKET ALERTS (FIFO Queue) ────────────┐\n";
    cout << "  │  Total Pending: " << count << " alerts"
         << string(34 - to_string(count).size(), ' ') << "│\n";
    cout << "  ├──────────────────────────────────────────────────────┤\n";
    int idx = front;
    for (int i = 0; i < count; i++) {
      string icon;
      if (alerts[idx].alertType == "PRICE_SPIKE")
        icon = "📈";
      else if (alerts[idx].alertType == "PRICE_DROP")
        icon = "📉";
      else if (alerts[idx].alertType == "VOLUME_SURGE")
        icon = "🔊";
      else
        icon = "📢";

      cout << "  │  " << icon << " [" << alerts[idx].symbol << "] "
           << alerts[idx].message.substr(0, 42) << "\n";
      idx = (idx + 1) % MAX_SIZE;
    }
    cout << "  └──────────────────────────────────────────────────────┘\n";
  }

  // Process (dequeue + display) next alert
  void processNext() {
    if (isEmpty()) {
      cout << "\n  📭 No alerts to process.\n";
      return;
    }
    MarketAlert alert = dequeue();
    string icon;
    if (alert.alertType == "PRICE_SPIKE")
      icon = "📈";
    else if (alert.alertType == "PRICE_DROP")
      icon = "📉";
    else if (alert.alertType == "VOLUME_SURGE")
      icon = "🔊";
    else
      icon = "📢";

    cout << "\n  ┌─── PROCESSING ALERT ────────────────────┐\n";
    cout << "  │  " << icon << " " << alert.symbol << "\n";
    cout << "  │  " << alert.message << "\n";
    cout << "  │  Type: " << alert.alertType << "\n";
    cout << "  └──────────────────────────────────────────┘\n";
  }
};

#endif // QUEUE_H
