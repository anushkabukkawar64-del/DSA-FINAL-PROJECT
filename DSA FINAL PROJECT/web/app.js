// ============================================================
// app.js - Front-End Logic mirroring C++ DSA Implementations
// Smart Stock Market Platform - Indian Stock Market
// ============================================================

// ─────────────────────────────────────────────
// 1. Initial State Data (Indian Stocks)
// ─────────────────────────────────────────────
let stocks = [
    { symbol: "RELIANCE",   company: "Reliance Industries",      sector: "Energy",         price: 1450.0,  growth: 3.2,  high52w: 1620.0, low52w: 1280.0, volume: 4500000 },
    { symbol: "TCS",        company: "Tata Consultancy Services",sector: "Technology",     price: 3850.0,  growth: 2.5,  high52w: 4200.0, low52w: 3400.0, volume: 1800000 },
    { symbol: "INFY",       company: "Infosys",                  sector: "Technology",     price: 1620.0,  growth: 1.8,  high52w: 1750.0, low52w: 1350.0, volume: 2900000 },
    { symbol: "HDFCBANK",   company: "HDFC Bank",                sector: "Banking",        price: 1680.0,  growth: 2.9,  high52w: 1800.0, low52w: 1420.0, volume: 5200000 },
    { symbol: "ICICIBANK",  company: "ICICI Bank",               sector: "Banking",        price: 1240.0,  growth: 3.5,  high52w: 1320.0, low52w: 1050.0, volume: 3800000 },
    { symbol: "SBIN",       company: "State Bank of India",      sector: "Banking",        price: 920.0,   growth: 4.1,  high52w: 980.0,  low52w: 710.0,  volume: 6100000 },
    { symbol: "BHARTIARTL", company: "Bharti Airtel",            sector: "Telecom",        price: 1870.0,  growth: 2.1,  high52w: 1950.0, low52w: 1490.0, volume: 2200000 },
    { symbol: "ITC",        company: "ITC Limited",              sector: "FMCG",            price: 510.0,   growth: 1.6,  high52w: 540.0,  low52w: 420.0,  volume: 8500000 },
    { symbol: "LT",         company: "Larsen & Toubro",          sector: "Infrastructure", price: 3620.0,  growth: 1.9,  high52w: 3900.0, low52w: 3100.0, volume: 1200000 },
    { symbol: "ADANIENT",   company: "Adani Enterprises",        sector: "Energy",         price: 2850.0,  growth: 5.3,  high52w: 3200.0, low52w: 2400.0, volume: 3100000 }
];

// Portfolio state
let cashBalance = 1000000.00;
let portfolioAVL = null; // root node
let transactionStack = []; // LIFO Stack
let alertQueue = []; // Circular queue replica array
const MAX_QUEUE_SIZE = 15;

// Graph details
const sectorGraph = {
    nodes: ["Banking", "Infrastructure", "Energy", "Technology", "FMCG", "Telecom"],
    adj: {
        "Banking": [{ dest: "Infrastructure", w: 0.8 }, { dest: "FMCG", w: 0.4 }, { dest: "Technology", w: 0.6 }],
        "Infrastructure": [{ dest: "Banking", w: 0.8 }, { dest: "Energy", w: 0.9 }],
        "Energy": [{ dest: "Infrastructure", w: 0.9 }, { dest: "Telecom", w: 0.5 }],
        "Technology": [{ dest: "Telecom", w: 0.75 }, { dest: "Banking", w: 0.6 }],
        "FMCG": [{ dest: "Banking", w: 0.4 }],
        "Telecom": [{ dest: "Technology", w: 0.75 }, { dest: "Energy", w: 0.5 }]
    }
};

// ─────────────────────────────────────────────
// 2. Custom DSA Classes (Direct Implementations)
// ─────────────────────────────────────────────

// O(1) Hash Map with Separate Chaining
class StockHashMap {
    constructor(capacity = 23) {
        this.capacity = capacity;
        this.table = Array(capacity).fill(null).map(() => []);
    }
    _hash(key) {
        let hash = 0;
        for (let i = 0; i < key.length; i++) {
            hash = (hash * 31 + key.charCodeAt(i)) % this.capacity;
        }
        return hash;
    }
    insert(key, value) {
        const idx = this._hash(key);
        const chain = this.table[idx];
        const exist = chain.find(item => item.key === key);
        if (exist) {
            exist.value = value;
        } else {
            chain.push({ key, value });
        }
    }
    search(key) {
        const idx = this._hash(key);
        const cell = this.table[idx].find(item => item.key === key);
        return cell ? cell.value : null;
    }
}

const stockMapInstance = new StockHashMap();

// AVL Portfolio node structure
class AVLNode {
    constructor(symbol, qty, avgPrice, currentPrice) {
        this.symbol = symbol;
        this.qty = qty;
        this.avgPrice = avgPrice;
        this.currentPrice = currentPrice;
        this.left = null;
        this.right = null;
        this.height = 1;
    }
    getInvestment() { return this.qty * this.avgPrice; }
    getCurrentValue() { return this.qty * this.currentPrice; }
    getPL() { return this.getCurrentValue() - this.getInvestment(); }
}

// AVL Portfolio Tree balancer helper
class AVLTree {
    static getHeight(node) { return node ? node.height : 0; }
    static getBalance(node) { return node ? this.getHeight(node.left) - this.getHeight(node.right) : 0; }
    static updateHeight(node) {
        if (node) node.height = 1 + Math.max(this.getHeight(node.left), this.getHeight(node.right));
    }
    static rotateRight(y) {
        let x = y.left;
        let T2 = x.right;
        x.right = y;
        y.left = T2;
        this.updateHeight(y);
        this.updateHeight(x);
        return x;
    }
    static rotateLeft(x) {
        let y = x.right;
        let T2 = y.left;
        y.left = x;
        x.right = T2;
        this.updateHeight(x);
        this.updateHeight(y);
        return y;
    }
    static insert(node, symbol, qty, avgPrice, currentPrice) {
        if (!node) return new AVLNode(symbol, qty, avgPrice, currentPrice);

        if (symbol < node.symbol) {
            node.left = this.insert(node.left, symbol, qty, avgPrice, currentPrice);
        } else if (symbol > node.symbol) {
            node.right = this.insert(node.right, symbol, qty, avgPrice, currentPrice);
        } else {
            // update existing
            const totalQty = node.qty + qty;
            if (totalQty > 0) {
                node.avgPrice = (node.getInvestment() + (qty * avgPrice)) / totalQty;
                node.qty = totalQty;
            }
            return node;
        }

        this.updateHeight(node);
        const balance = this.getBalance(node);

        // LL
        if (balance > 1 && symbol < node.left.symbol) return this.rotateRight(node);
        // RR
        if (balance < -1 && symbol > node.right.symbol) return this.rotateLeft(node);
        // LR
        if (balance > 1 && symbol > node.left.symbol) {
            node.left = this.rotateLeft(node.left);
            return this.rotateRight(node);
        }
        // RL
        if (balance < -1 && symbol < node.right.symbol) {
            node.right = this.rotateRight(node.right);
            return this.rotateLeft(node);
        }

        return node;
    }
    static getMinValueNode(node) {
        let curr = node;
        while (curr.left) curr = curr.left;
        return curr;
    }
    static delete(node, symbol, removedRef = {}) {
        if (!node) return null;

        if (symbol < node.symbol) {
            node.left = this.delete(node.left, symbol, removedRef);
        } else if (symbol > node.symbol) {
            node.right = this.delete(node.right, symbol, removedRef);
        } else {
            removedRef.qty = node.qty;
            removedRef.avgPrice = node.avgPrice;
            
            if (!node.left || !node.right) {
                let temp = node.left ? node.left : node.right;
                if (!temp) {
                    node = null;
                } else {
                    node = temp;
                }
            } else {
                let temp = this.getMinValueNode(node.right);
                node.symbol = temp.symbol;
                node.qty = temp.qty;
                node.avgPrice = temp.avgPrice;
                node.currentPrice = temp.currentPrice;
                node.right = this.delete(node.right, temp.symbol, removedRef);
            }
        }

        if (!node) return null;

        this.updateHeight(node);
        const balance = this.getBalance(node);

        if (balance > 1 && this.getBalance(node.left) >= 0) return this.rotateRight(node);
        if (balance > 1 && this.getBalance(node.left) < 0) {
            node.left = this.rotateLeft(node.left);
            return this.rotateRight(node);
        }
        if (balance < -1 && this.getBalance(node.right) <= 0) return this.rotateLeft(node);
        if (balance < -1 && this.getBalance(node.right) > 0) {
            node.right = this.rotateRight(node.right);
            return this.rotateLeft(node);
        }

        return node;
    }
    static search(node, symbol) {
        if (!node || node.symbol === symbol) return node;
        if (symbol < node.symbol) return this.search(node.left, symbol);
        return this.search(node.right, symbol);
    }
    static toArray(node, arr = []) {
        if (!node) return arr;
        this.toArray(node.left, arr);
        arr.push(node);
        this.toArray(node.right, arr);
        return arr;
    }
    static updatePrice(node, symbol, price) {
        if (!node) return;
        if (node.symbol === symbol) {
            node.currentPrice = price;
            return;
        }
        this.updatePrice(node.left, symbol, price);
        this.updatePrice(node.right, symbol, price);
    }
}

// Binary Search Tree (for Price sorting and traversal)
class BSTNode {
    constructor(stock) {
        this.stock = stock;
        this.left = null;
        this.right = null;
    }
}
class BST {
    static insert(node, stock) {
        if (!node) return new BSTNode(stock);
        if (stock.price < node.stock.price) {
            node.left = this.insert(node.left, stock);
        } else {
            node.right = this.insert(node.right, stock);
        }
        return node;
    }
    static inorder(node, arr = []) {
        if (!node) return arr;
        this.inorder(node.left, arr);
        arr.push(node.stock);
        this.inorder(node.right, arr);
        return arr;
    }
    static preorder(node, arr = []) {
        if (!node) return arr;
        arr.push(node.stock);
        this.preorder(node.left, arr);
        this.preorder(node.right, arr);
        return arr;
    }
    static postorder(node, arr = []) {
        if (!node) return arr;
        this.postorder(node.left, arr);
        this.postorder(node.right, arr);
        arr.push(node.stock);
        return arr;
    }
}

// ─────────────────────────────────────────────
// 3. UI Display & Interactivity Engine
// ─────────────────────────────────────────────

// Initialize values
function initSystem() {
    // Populate O(1) hash map
    stocks.forEach(s => stockMapInstance.insert(s.symbol, s));

    // Populate trade form selectors
    const select = document.getElementById("trade-symbol");
    select.innerHTML = "";
    stocks.forEach(s => {
        const option = document.createElement("option");
        option.value = s.symbol;
        option.textContent = `${s.symbol} (₹${s.price.toFixed(2)})`;
        select.appendChild(option);
    });

    renderMarketBoard();
    updatePortfolioUI();
    renderAlerts();
    updateEstimatedCost();
    renderHashTable();
}

// Render stock board
let currentSort = "symbol";
function renderMarketBoard() {
    const tbody = document.getElementById("market-tbody");
    tbody.innerHTML = "";

    // Copy array
    let sorted = [...stocks];

    // Sorting algorithm selections based on DSA
    if (currentSort === "price") {
        // Bubble sort demonstration
        for (let i = 0; i < sorted.length; i++) {
            for (let j = 0; j < sorted.length - i - 1; j++) {
                if (sorted[j].price > sorted[j+1].price) {
                    let temp = sorted[j];
                    sorted[j] = sorted[j+1];
                    sorted[j+1] = temp;
                }
            }
        }
    } else if (currentSort === "growth") {
        // Selection sort demonstration
        for (let i = 0; i < sorted.length; i++) {
            let maxIdx = i;
            for (let j = i + 1; j < sorted.length; j++) {
                if (sorted[j].growth > sorted[maxIdx].growth) maxIdx = j;
            }
            let temp = sorted[i];
            sorted[i] = sorted[maxIdx];
            sorted[maxIdx] = temp;
        }
    } else if (currentSort === "volume") {
        // Quick sort helper
        sorted = quickSortByVolume(sorted);
    } else {
        // Symbol merge sort alphabetically
        sorted.sort((a,b) => a.symbol.localeCompare(b.symbol));
    }

    sorted.forEach(s => {
        const tr = document.createElement("tr");
        const isPositive = s.growth >= 0;
        const trendSign = isPositive ? "+" : "";
        const pillClass = isPositive ? "positive" : "negative";

        tr.innerHTML = `
            <td class="font-bold">${s.symbol}</td>
            <td>${s.company}</td>
            <td><span class="chip">${s.sector}</span></td>
            <td class="text-right font-bold">₹${s.price.toFixed(2)}</td>
            <td class="text-right">
                <span class="change-pill ${pillClass}">${trendSign}${s.growth.toFixed(2)}%</span>
            </td>
            <td class="text-right text-muted font-bold" style="font-size:12px;">₹${s.low52w.toFixed(0)} - ₹${s.high52w.toFixed(0)}</td>
            <td class="text-center">
                <button class="btn btn-secondary btn-sm" onclick="prefillTrade('${s.symbol}', 'BUY')">Buy</button>
                <button class="btn btn-secondary btn-sm" style="color:var(--accent-red);" onclick="prefillTrade('${s.symbol}', 'SELL')">Sell</button>
            </td>
        `;
        tbody.appendChild(tr);
    });
}

function quickSortByVolume(arr) {
    if (arr.length <= 1) return arr;
    let pivot = arr[arr.length - 1];
    let left = [];
    let right = [];
    for (let i = 0; i < arr.length - 1; i++) {
        if (arr[i].volume > pivot.volume) {
            left.push(arr[i]);
        } else {
            right.push(arr[i]);
        }
    }
    return [...quickSortByVolume(left), pivot, ...quickSortByVolume(right)];
}

function sortMarket(criteria) {
    currentSort = criteria;
    // Highlight active sorting tabs
    document.querySelectorAll(".sort-controls .chip").forEach(c => c.classList.remove("active"));
    event.target.classList.add("active");
    renderMarketBoard();
}

function prefillTrade(symbol, type) {
    document.getElementById("trade-symbol").value = symbol;
    document.querySelector(`input[name="trade-type"][value="${type}"]`).checked = true;
    updateEstimatedCost();
    document.getElementById("trade-symbol").focus();
}

// Render portfolio
function updatePortfolioUI() {
    const tbody = document.getElementById("portfolio-tbody");
    tbody.innerHTML = "";

    const items = AVLTree.toArray(portfolioAVL);
    
    let totalInvVal = 0;
    let totalCurVal = 0;

    if (items.length === 0) {
        tbody.innerHTML = `<tr><td colspan="7" class="text-center text-muted" style="padding:24px;">No stocks in your portfolio. Place a Buy order to start.</td></tr>`;
    } else {
        items.forEach(node => {
            const tr = document.createElement("tr");
            const pl = node.getPL();
            const plPct = node.getInvestment() > 0 ? (pl / node.getInvestment() * 100) : 0;
            const sign = pl >= 0 ? "+" : "";
            const textClass = pl >= 0 ? "positive" : "negative";

            totalInvVal += node.getInvestment();
            totalCurVal += node.getCurrentValue();

            tr.innerHTML = `
                <td class="font-bold">${node.symbol}</td>
                <td class="text-right">${node.qty}</td>
                <td class="text-right">₹${node.avgPrice.toFixed(2)}</td>
                <td class="text-right font-bold">₹${node.currentPrice.toFixed(2)}</td>
                <td class="text-right">₹${node.getInvestment().toFixed(2)}</td>
                <td class="text-right font-bold">₹${node.getCurrentValue().toFixed(2)}</td>
                <td class="text-right font-bold ${textClass}">${sign}₹${pl.toFixed(2)} (${sign}${plPct.toFixed(1)}%)</td>
            `;
            tbody.appendChild(tr);
        });
    }

    // Update metrics cards
    document.getElementById("total-investment").textContent = `₹${totalInvVal.toLocaleString("en-IN", {minimumFractionDigits:2, maximumFractionDigits:2})}`;
    document.getElementById("portfolio-value").textContent = `₹${totalCurVal.toLocaleString("en-IN", {minimumFractionDigits:2, maximumFractionDigits:2})}`;
    document.getElementById("cash-balance").textContent = `₹${cashBalance.toLocaleString("en-IN", {minimumFractionDigits:2, maximumFractionDigits:2})}`;

    const netPL = totalCurVal - totalInvVal;
    const netPLPct = totalInvVal > 0 ? (netPL / totalInvVal * 100) : 0;
    const plSign = netPL >= 0 ? "+" : "";
    const netEl = document.getElementById("net-pl");
    netEl.textContent = `${plSign}₹${netPL.toLocaleString("en-IN", {minimumFractionDigits:2, maximumFractionDigits:2})} (${plSign}${netPLPct.toFixed(2)}%)`;
    if (netPL >= 0) {
        netEl.className = "metric-val positive";
    } else {
        netEl.className = "metric-val negative";
    }
}

// Estimated Cost calculations
function updateEstimatedCost() {
    const symbol = document.getElementById("trade-symbol").value;
    const qty = parseInt(document.getElementById("trade-qty").value) || 0;
    const stock = stockMapInstance.search(symbol);
    if (stock) {
        const est = stock.price * qty;
        document.getElementById("estimated-cost-val").textContent = `₹${est.toLocaleString("en-IN", {minimumFractionDigits:2})}`;
    }
}

// Trade Operations
function executeTrade() {
    const symbol = document.getElementById("trade-symbol").value;
    const qty = parseInt(document.getElementById("trade-qty").value);
    const type = document.querySelector('input[name="trade-type"]:checked').value;

    const stock = stockMapInstance.search(symbol);
    if (!stock) return;

    const cost = stock.price * qty;

    if (type === "BUY") {
        if (cost > cashBalance) {
            pushAlert("TRANSACTION_FAILED", `Insufficient funds to buy ${qty} shares of ${symbol}.`, "price-drop");
            alert("Insufficient funds!");
            return;
        }
        cashBalance -= cost;
        portfolioAVL = AVLTree.insert(portfolioAVL, symbol, qty, stock.price, stock.price);
        
        // Push LIFO transaction onto stack
        transactionStack.push({ type: "BUY", symbol, qty, price: stock.price });
        pushAlert("BUY_ORDER", `Bought ${qty} shares of ${stock.company} @ ₹${stock.price.toFixed(2)}`, "price-spike");
    } else {
        // Sell logic
        const holding = AVLTree.search(portfolioAVL, symbol);
        if (!holding || holding.qty < qty) {
            pushAlert("TRANSACTION_FAILED", `Cannot sell ${qty} shares of ${symbol}. Insufficient holdings.`, "price-drop");
            alert("Insufficient shares owned!");
            return;
        }

        let originalPrice = holding.avgPrice;
        let removedRef = {};
        portfolioAVL = AVLTree.delete(portfolioAVL, symbol, removedRef);

        // If we only sold a portion, insert the remainder back
        const remaining = holding.qty - qty;
        if (remaining > 0) {
            portfolioAVL = AVLTree.insert(portfolioAVL, symbol, remaining, originalPrice, stock.price);
        }

        cashBalance += cost;

        // Push LIFO transaction onto stack
        transactionStack.push({ type: "SELL", symbol, qty, price: originalPrice });
        
        const pl = qty * (stock.price - originalPrice);
        const sign = pl >= 0 ? "Profit" : "Loss";
        pushAlert("SELL_ORDER", `Sold ${qty} shares of ${stock.company}. Realized ${sign} of ₹${Math.abs(pl).toFixed(2)}`, pl >= 0 ? "price-spike" : "price-drop");
    }

    updatePortfolioUI();
    updateEstimatedCost();
    renderBSTVisual();
}

// Revert last action using Stack (Undo)
function undoLastTrade() {
    if (transactionStack.length === 0) {
        alert("No actions to undo!");
        return;
    }
    const lastAction = transactionStack.pop();
    const stock = stockMapInstance.search(lastAction.symbol);
    
    if (lastAction.type === "BUY") {
        // Undo BUY means we sell/remove those shares back at same price
        let removedRef = {};
        portfolioAVL = AVLTree.delete(portfolioAVL, lastAction.symbol, removedRef);
        const remaining = (removedRef.qty || 0) - lastAction.qty;
        if (remaining > 0) {
            portfolioAVL = AVLTree.insert(portfolioAVL, lastAction.symbol, remaining, lastAction.price, stock.price);
        }
        cashBalance += (lastAction.qty * lastAction.price);
        pushAlert("UNDO", `Reverted purchase of ${lastAction.qty} shares of ${lastAction.symbol}`, "info");
    } else {
        // Undo SELL means we buy back the shares at original price
        cashBalance -= (lastAction.qty * (stock ? stock.price : lastAction.price));
        portfolioAVL = AVLTree.insert(portfolioAVL, lastAction.symbol, lastAction.qty, lastAction.price, stock.price);
        pushAlert("UNDO", `Reverted sale of ${lastAction.qty} shares of ${lastAction.symbol}`, "info");
    }

    updatePortfolioUI();
}

// ─────────────────────────────────────────────
// 4. Alerts circular queue system
// ─────────────────────────────────────────────
function pushAlert(type, message, statusClass) {
    if (alertQueue.length >= MAX_QUEUE_SIZE) {
        alertQueue.shift(); // Dequeue oldest
    }
    alertQueue.push({ type, message, time: new Date().toLocaleTimeString(), statusClass });
    renderAlerts();
}

function renderAlerts() {
    const list = document.getElementById("alerts-list");
    list.innerHTML = "";

    if (alertQueue.length === 0) {
        list.innerHTML = `<p class="text-center text-muted" style="padding:16px;">Queue empty. Alerts arrive on ticker update.</p>`;
        return;
    }

    // Display from rear to front (newest first for readability)
    for (let i = alertQueue.length - 1; i >= 0; i--) {
        const item = alertQueue[i];
        const div = document.createElement("div");
        div.className = `alert-item ${item.statusClass}`;
        
        let icon = "📢";
        if (item.statusClass === "price-spike") icon = "📈";
        if (item.statusClass === "price-drop") icon = "📉";
        if (item.statusClass === "info") icon = "🔄";

        div.innerHTML = `
            <span style="font-size:18px;">${icon}</span>
            <div style="flex:1;">
                <strong>[${item.type}]</strong> ${item.message}
                <div style="font-size:10px; color:var(--text-muted); margin-top:2px;">${item.time}</div>
            </div>
        `;
        list.appendChild(div);
    }
}

function clearAlerts() {
    alertQueue = [];
    renderAlerts();
}

// ─────────────────────────────────────────────
// 5. Market Fluctuations Ticker Update
// ─────────────────────────────────────────────
document.getElementById("tick-btn").addEventListener("click", () => {
    // Fluctuates stock prices randomly
    stocks.forEach(s => {
        const pct = ((Math.random() * 6) - 2.8); // -2.8% to +3.2%
        s.price = s.price * (1 + pct / 100);
        s.growth = pct;
        if (s.price > s.high52w) s.high52w = s.price;
        if (s.price < s.low52w) s.low52w = s.price;
        s.volume += Math.floor(Math.random() * 200000);

        stockMapInstance.insert(s.symbol, s);
        AVLTree.updatePrice(portfolioAVL, s.symbol, s.price);
    });

    // Compute average market movement
    const totalGrowth = stocks.reduce((acc, curr) => acc + curr.growth, 0);
    const avgGrowth = totalGrowth / stocks.length;
    const badge = document.getElementById("market-trend-badge");

    if (avgGrowth > 1.0) {
        badge.className = "trend-badge bullish";
        badge.textContent = "STRONGLY BULLISH 🚀";
        pushAlert("MARKET_BOOST", "Market sentiment is positive. IT & Infrastructure sectors surging.", "price-spike");
    } else if (avgGrowth > 0) {
        badge.className = "trend-badge bullish";
        badge.textContent = "BULLISH 📈";
    } else {
        badge.className = "trend-badge bearish";
        badge.textContent = "BEARISH 📉";
        pushAlert("MARKET_DIP", "Market correction under way. High profit-booking in Banking.", "price-drop");
    }

    // Trigger alert updates for major movements
    stocks.forEach(s => {
        if (s.growth > 2.5) {
            pushAlert("SPIKE_DETECTOR", `${s.symbol} jumped +${s.growth.toFixed(1)}%! Breakout candidate.`, "price-spike");
        } else if (s.growth < -2.2) {
            pushAlert("VOLUME_SELL", `${s.symbol} experiencing volume pressure: down -${Math.abs(s.growth).toFixed(1)}%`, "price-drop");
        }
    });

    renderMarketBoard();
    updatePortfolioUI();
    renderBSTVisual();
});

// ─────────────────────────────────────────────
// 6. Graph BFS / DFS Module
// ─────────────────────────────────────────────
function runGraphTraversal(startNode) {
    // Reset highlights
    document.querySelectorAll(".sector-node").forEach(n => n.classList.remove("highlighted"));

    // Highlight starting sector node
    document.getElementById(`node-${startNode}`).classList.add("highlighted");

    // Breadth First Search (BFS) implementation
    let bfsResult = [];
    let visited = {};
    let queue = [startNode];
    visited[startNode] = true;

    while (queue.length > 0) {
        let curr = queue.shift();
        bfsResult.push(curr);

        let neighbors = sectorGraph.adj[curr] || [];
        neighbors.forEach(n => {
            if (!visited[n.dest]) {
                visited[n.dest] = true;
                queue.push(n.dest);
            }
        });
    }

    // Depth First Search (DFS) implementation
    let dfsResult = [];
    let dfsVisited = {};
    function dfsHelper(node) {
        dfsVisited[node] = true;
        dfsResult.push(node);
        let neighbors = sectorGraph.adj[node] || [];
        neighbors.forEach(n => {
            if (!dfsVisited[n.dest]) {
                dfsHelper(n.dest);
            }
        });
    }
    dfsHelper(startNode);

    // Render results
    document.getElementById("bfs-output").textContent = bfsResult.join(" ➔ ");
    document.getElementById("dfs-output").textContent = dfsResult.join(" ➔ ");

    // Diversification advice
    const directLinks = (sectorGraph.adj[startNode] || []).map(n => n.dest);
    const nonLinked = sectorGraph.nodes.filter(n => n !== startNode && !directLinks.includes(n));
    document.getElementById("diversify-output").textContent = `Avoid ${directLinks.join(", ")}. Invest in: ${nonLinked.join(" or ")} to spread sector risks.`;
}

// ─────────────────────────────────────────────
// 7. BST Price Tree Traversal Render
// ─────────────────────────────────────────────
function renderBSTVisual() {
    let rootNode = null;
    stocks.forEach(s => {
        rootNode = BST.insert(rootNode, s);
    });

    const container = document.getElementById("bst-tree-container");
    container.innerHTML = "";

    // Render a mini visual node map of root and branches
    if (rootNode) {
        container.innerHTML = `
            <div style="display:flex; flex-direction:column; align-items:center; gap:20px;">
                <div class="tree-node-circle root" title="${rootNode.stock.company}">
                    ${rootNode.stock.symbol}
                </div>
                <div style="display:flex; gap:60px;">
                    <div style="display:flex; flex-direction:column; align-items:center;">
                        <span style="font-size:10px; color:var(--text-muted);">Left (Lower Price)</span>
                        <div class="tree-node-circle" style="border-style:dashed;">
                            ${rootNode.left ? rootNode.left.stock.symbol : "Null"}
                        </div>
                    </div>
                    <div style="display:flex; flex-direction:column; align-items:center;">
                        <span style="font-size:10px; color:var(--text-muted);">Right (Higher Price)</span>
                        <div class="tree-node-circle" style="border-style:dashed;">
                            ${rootNode.right ? rootNode.right.stock.symbol : "Null"}
                        </div>
                    </div>
                </div>
            </div>
        `;
    }
}

function traverseBST(type) {
    let rootNode = null;
    stocks.forEach(s => {
        rootNode = BST.insert(rootNode, s);
    });

    let result = [];
    if (type === "inorder") {
        result = BST.inorder(rootNode);
    } else if (type === "preorder") {
        result = BST.preorder(rootNode);
    } else {
        result = BST.postorder(rootNode);
    }

    const outputString = result.map(s => `${s.symbol} (₹${s.price.toFixed(0)})`).join(" ➔ ");
    document.getElementById("bst-traversal-output").textContent = `BST ${type.toUpperCase()}:\n${outputString}`;
}

// ─────────────────────────────────────────────
// 8. AVL Holdings Modal Visualization
// ─────────────────────────────────────────────
function showAvlStructure() {
    const modal = document.getElementById("avl-modal");
    const container = document.getElementById("avl-tree-display");
    container.innerHTML = "";

    if (!portfolioAVL) {
        container.innerHTML = `<div class="text-center text-muted">Portfolio holds no stocks yet to balance.</div>`;
    } else {
        // Simple HTML list hierarchical structure representing balance levels
        function buildHtmlTree(node) {
            if (!node) return "";
            return `
                <div class="tree-node-visual">
                    <div class="tree-node-circle ${node.height > 2 ? 'balance-alert' : ''}">
                        ${node.symbol}
                    </div>
                    <div style="font-size:10px; font-weight:700; margin-top:2px;">Height: ${node.height}</div>
                    <div style="display:flex; gap:24px; margin-top:12px;">
                        ${node.left ? buildHtmlTree(node.left) : '<div class="text-muted" style="font-size:9px;">L:Null</div>'}
                        ${node.right ? buildHtmlTree(node.right) : '<div class="text-muted" style="font-size:9px;">R:Null</div>'}
                    </div>
                </div>
            `;
        }
        container.innerHTML = buildHtmlTree(portfolioAVL);
    }

    modal.classList.add("active");
}

function closeAvlModal() {
    document.getElementById("avl-modal").classList.remove("active");
}

// ─────────────────────────────────────────────
// 9. Hash Table Diagnostic Table
// ─────────────────────────────────────────────
function renderHashTable() {
    const container = document.getElementById("hash-table-container");
    container.innerHTML = "";

    for (let i = 0; i < stockMapInstance.capacity; i++) {
        const chain = stockMapInstance.table[i];
        if (chain.length > 0) {
            const div = document.createElement("div");
            div.className = "hash-slot";
            const syms = chain.map(c => c.key).join(" ➜ ");
            div.innerHTML = `
                <span class="bucket-num">Bucket #${i}</span>
                <span class="bucket-chain">${syms}</span>
            `;
            container.appendChild(div);
        }
    }
}

// Tab switcher
function switchDsaTab(tabId) {
    document.querySelectorAll(".dsa-tab-content").forEach(c => c.classList.remove("active"));
    document.querySelectorAll(".dsa-tabs .tab-btn").forEach(b => b.classList.remove("active"));
    
    document.getElementById(tabId).classList.add("active");
    event.target.classList.add("active");
}

// Auto Init
window.onload = () => {
    initSystem();
    runGraphTraversal("Banking");
    renderBSTVisual();
};
