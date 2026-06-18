// ============================================================
// Graph.h - Sector Relationship Graph (BFS and DFS)
// Smart Stock Market Platform - Indian Stock Market
// B.Tech CSE 2025-29 | DSA with C++ | Sem II | ITM Skills University
// ============================================================

#ifndef GRAPH_H
#define GRAPH_H

#include <string>
#include <iostream>
#include <iomanip>

using namespace std;

// ─────────────────────────────────────────────
// Adjacency List Node
// ─────────────────────────────────────────────
struct AdjListNode {
    int destIndex;
    double correlation; // Edge weight: relation factor (e.g. 0.8 correlation)
    AdjListNode* next;

    AdjListNode(int dest, double corr) : destIndex(dest), correlation(corr), next(nullptr) {}
};

// ─────────────────────────────────────────────
// Sector Graph using Adjacency List
// BFS & DFS implementation for Sector Relationships
// ─────────────────────────────────────────────
class SectorGraph {
private:
    static const int MAX_VERTICES = 10;
    string vertices[MAX_VERTICES];
    AdjListNode* adjList[MAX_VERTICES];
    int numVertices;

    // Helper to find vertex index by name
    int getVertexIndex(const string& name) const {
        for (int i = 0; i < numVertices; i++) {
            if (vertices[i] == name) return i;
        }
        return -1;
    }

    // DFS Recursive helper
    void DFSRecursive(int v, bool visited[]) const {
        visited[v] = true;
        cout << " ──> " << vertices[v];

        AdjListNode* temp = adjList[v];
        while (temp) {
            if (!visited[temp->destIndex]) {
                DFSRecursive(temp->destIndex, visited);
            }
            temp = temp->next;
        }
    }

public:
    SectorGraph() : numVertices(0) {
        for (int i = 0; i < MAX_VERTICES; i++) {
            adjList[i] = nullptr;
        }
    }

    ~SectorGraph() {
        for (int i = 0; i < numVertices; i++) {
            AdjListNode* curr = adjList[i];
            while (curr) {
                AdjListNode* temp = curr;
                curr = curr->next;
                delete temp;
            }
        }
    }

    // Add Sector Vertex
    bool addSector(const string& sectorName) {
        if (numVertices >= MAX_VERTICES) return false;
        if (getVertexIndex(sectorName) != -1) return false; // Already exists

        vertices[numVertices] = sectorName;
        adjList[numVertices] = nullptr;
        numVertices++;
        return true;
    }

    // Add Edge (Sector Correlation / Dependency)
    // Undirected graph representation of sector relationships
    bool addRelationship(const string& src, const string& dest, double correlation) {
        int srcIdx = getVertexIndex(src);
        int destIdx = getVertexIndex(dest);

        if (srcIdx == -1 || destIdx == -1) return false;

        // Add edge src -> dest
        AdjListNode* newNode1 = new AdjListNode(destIdx, correlation);
        newNode1->next = adjList[srcIdx];
        adjList[srcIdx] = newNode1;

        // Add edge dest -> src (Undirected relationship)
        AdjListNode* newNode2 = new AdjListNode(srcIdx, correlation);
        newNode2->next = adjList[destIdx];
        adjList[destIdx] = newNode2;

        return true;
    }

    // Breadth-First Search (BFS)
    // Shows breadth-wise propagation of sector impact (e.g. interest rate hike)
    void BFS(const string& startSector) const {
        int startIdx = getVertexIndex(startSector);
        if (startIdx == -1) {
            cout << "  ✗ Sector \"" << startSector << "\" not found in graph.\n";
            return;
        }

        bool visited[MAX_VERTICES] = {false};
        int queue[MAX_VERTICES];
        int front = 0, rear = 0;

        // Enqueue start vertex
        queue[rear++] = startIdx;
        visited[startIdx] = true;

        cout << "\n  🌐 BFS Sector Impact Propagation starting from \"" << startSector << "\":\n  ";
        cout << "[Start] " << startSector;

        while (front < rear) {
            int currIdx = queue[front++];
            
            AdjListNode* temp = adjList[currIdx];
            while (temp) {
                int adjIdx = temp->destIndex;
                if (!visited[adjIdx]) {
                    visited[adjIdx] = true;
                    queue[rear++] = adjIdx;
                    cout << " ──> " << vertices[adjIdx] << " (Corr: " << fixed << setprecision(1) << temp->correlation << ")";
                }
                temp = temp->next;
            }
        }
        cout << "\n";
    }

    // Depth-First Search (DFS)
    // Demonstrates deep chain of correlation for risk assessment
    void DFS(const string& startSector) const {
        int startIdx = getVertexIndex(startSector);
        if (startIdx == -1) {
            cout << "  ✗ Sector \"" << startSector << "\" not found in graph.\n";
            return;
        }

        bool visited[MAX_VERTICES] = {false};
        cout << "\n  🕸 DFS Sector Chain Exploration starting from \"" << startSector << "\":\n  ";
        cout << "[Start]";
        DFSRecursive(startIdx, visited);
        cout << "\n";
    }

    // Display Adjacency List
    void displayGraph() const {
        cout << "\n  ┌───────── SECTOR RELATIONSHIP GRAPH ─────────┐\n";
        cout << "  │ (Showing sector dependencies and correlation) │\n";
        cout << "  ├─────────────────────────────────────────────┤\n";
        for (int i = 0; i < numVertices; i++) {
            cout << "  │ " << left << setw(15) << vertices[i] << " ➜ ";
            AdjListNode* temp = adjList[i];
            if (!temp) {
                cout << "Isolated Sector";
            }
            while (temp) {
                cout << vertices[temp->destIndex] << " (" << fixed << setprecision(1) << temp->correlation << ")";
                if (temp->next) cout << ", ";
                temp = temp->next;
            }
            cout << "\n";
        }
        cout << "  └─────────────────────────────────────────────┘\n";
    }

    // Initialize Default Indian Market Sector Graph
    void loadDefaultRelationships() {
        // Add vertices (NSE sectors)
        addSector("Banking");
        addSector("Infrastructure");
        addSector("Energy");
        addSector("Technology");
        addSector("FMCG");
        addSector("Telecom");

        // Add relationships with correlation weight (0.0 to 1.0)
        addRelationship("Banking", "Infrastructure", 0.8); // Infra projects need funding
        addRelationship("Banking", "FMCG", 0.4);           // FMCG is cash-rich, less dependent
        addRelationship("Infrastructure", "Energy", 0.9);   // Infra projects need power/energy
        addRelationship("Technology", "Telecom", 0.75);     // IT companies rely on 5G/Fiber infra
        addRelationship("Energy", "Telecom", 0.5);          // Standard industrial link
        addRelationship("Banking", "Technology", 0.6);      // Fintech & digitisation connection
    }

    // Get Recommendations: Find sectors least related to startSector to diversify portfolio
    void suggestDiversification(const string& heldSector) const {
        int idx = getVertexIndex(heldSector);
        if (idx == -1) return;

        cout << "\n  💡 Portfolio Diversification Suggestions (Based on Sector Graph):\n";
        cout << "   - You hold holdings in: " << heldSector << "\n";
        cout << "   - Direct high-risk correlation links to avoid over-exposure:\n";

        bool linked[MAX_VERTICES] = {false};
        AdjListNode* temp = adjList[idx];
        while (temp) {
            linked[temp->destIndex] = true;
            cout << "     ⚠️ " << vertices[temp->destIndex] << " (Directly linked with correlation weight " << temp->correlation << ")\n";
            temp = temp->next;
        }

        cout << "   - Suggested non-correlated sectors for diversification:\n";
        bool hasSuggestion = false;
        for (int i = 0; i < numVertices; i++) {
            if (i != idx && !linked[i]) {
                cout << "     ✅ " << vertices[i] << " (No direct link/Very low correlation)\n";
                hasSuggestion = true;
            }
        }
        if (!hasSuggestion) {
            cout << "     All sectors are currently related. Consider gold/fixed income assets!\n";
        }
    }
};

#endif // GRAPH_H
