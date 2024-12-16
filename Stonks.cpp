#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <iomanip>
#include <random>
#include <stdexcept>
#include <algorithm>

using namespace std;

// Linked List for Price History
struct PriceNode {
    double price;
    PriceNode* next;

    PriceNode(double p) : price(p), next(nullptr) {}
};

// Stock Class
class Stock {
private:
    string symbol;
    double currentPrice;
    int availableShares;
    PriceNode* priceHistoryHead;

public:
    Stock() : symbol(""), currentPrice(0.0), availableShares(0), priceHistoryHead(nullptr) {}

    Stock(string sym, double price, int shares) :
        symbol(sym), currentPrice(price), availableShares(shares), priceHistoryHead(new PriceNode(price)) {}

    ~Stock() {
        PriceNode* current = priceHistoryHead;
        while (current != nullptr) {
            PriceNode* next = current->next;
            delete current;
            current = next;
        }
    }

    string getSymbol() const { return symbol; }
    double getCurrentPrice() const { return currentPrice; }
    int getAvailableShares() const { return availableShares; }

    void updatePrice() {
    // Generate a random price change between -2.0 and 2.0
    srand(time(0));  // Only call srand once in main, but for now in the function
    double change = (rand() % 401 - 200) / 100.0;  // Change between -2.00 and +2.00

    // Debugging: Print the random change to verify it's being generated correctly
    cout << "Generated change: " << change << endl;

    // Update the current price ensuring it doesn't fall below 0.01
    currentPrice = max(0.01, currentPrice + change);

    // Debugging print to see if the price is changing
    cout << "Updated price: " << fixed << setprecision(2) << currentPrice << endl;

    // Create a new node with the updated price
    PriceNode* newNode = new PriceNode(currentPrice);

    // Insert the new node at the head of the list
    newNode->next = priceHistoryHead;
    priceHistoryHead = newNode; // Now the head points to the new node
}


    void buyShares(int shares) {
        if (shares <= availableShares) {
            availableShares -= shares;
        } else {
            throw runtime_error("Not enough shares available");
        }
    }

    void sellShares(int shares) {
        availableShares += shares;
    }

 void displayPriceHistory() const {
    cout << "Price history for " << symbol << ":\n";
    PriceNode* current = priceHistoryHead;
    
    // Check if the list is empty
    if (current == nullptr) {
        cout << "No price history available.\n";
        return;
    }

    // Display the prices
    int count = 0; // Counter to help with detecting cycles
    while (current != nullptr) {
        cout << fixed << setprecision(2) << current->price << " ";
        current = current->next;
        
        // For debugging, print the node count
        count++;
        if (count > 10) { // Arbitrary limit to detect infinite loop
            cout << "\nInfinite loop detected! Breaking.";
            break;
        }
    }
    cout << endl;
}
};

// Binary Search Tree for Portfolio Holdings
struct TreeNode {
    string symbol;
    int shares;
    TreeNode* left;
    TreeNode* right;

    TreeNode(string sym, int sh) : symbol(sym), shares(sh), left(nullptr), right(nullptr) {}
};

class Portfolio {
private:
    TreeNode* root;
    double cash;
    vector<string> transactionHistory;

    void insert(TreeNode*& node, const string& symbol, int shares) {
        if (node == nullptr) {
            node = new TreeNode(symbol, shares);
        } else if (symbol < node->symbol) {
            insert(node->left, symbol, shares);
        } else if (symbol > node->symbol) {
            insert(node->right, symbol, shares);
        } else {
            node->shares += shares; // Update shares if symbol already exists
        }
    }

    void display(TreeNode* node) const {
        if (node != nullptr) {
            display(node->left);
            cout << node->symbol << ": " << node->shares << " shares\n";
            display(node->right);
        }
    }
    TreeNode* findMin(TreeNode* node) {
    while (node->left != nullptr) {
        node = node->left;
    }
    return node;
}

    TreeNode* deleteNode(TreeNode* node, const string& symbol) {
    if (node == nullptr) {
        return node;
    }
    
    // Traverse the tree to find the node to delete
    if (symbol < node->symbol) {
        node->left = deleteNode(node->left, symbol);
    } else if (symbol > node->symbol) {
        node->right = deleteNode(node->right, symbol);
    } else {
        // Node found, handle deletion
        if (node->left == nullptr) {
            TreeNode* temp = node->right;
            delete node;
            return temp;
        } else if (node->right == nullptr) {
            TreeNode* temp = node->left;
            delete node;
            return temp;
        } else {
            // Node with two children: Find the inorder successor
            TreeNode* successor = findMin(node->right);
            node->symbol = successor->symbol;
            node->shares = successor->shares;
            node->right = deleteNode(node->right, successor->symbol);
        }
    }
    return node;
    }

public:
    Portfolio(double initialCash = 10000.0) : root(nullptr), cash(initialCash) {}
    void buyStock(Stock& stock, int shares) {
        double cost = shares * stock.getCurrentPrice();
        if (cost <= cash) {
            stock.buyShares(shares);
            insert(root, stock.getSymbol(), shares);
            cash -= cost;

            // Record transaction
            string transaction = "Bought " + to_string(shares) + " shares of " +
                stock.getSymbol() + " at $" + to_string(stock.getCurrentPrice());
            transactionHistory.push_back(transaction);
        } else {
            throw runtime_error("Insufficient funds");
        }
    }
    void updateShares(TreeNode*& node, const string& symbol, int deltaShares) {
    if (node == nullptr) return;

    if (symbol < node->symbol) {
        updateShares(node->left, symbol, deltaShares);
    } else if (symbol > node->symbol) {
        updateShares(node->right, symbol, deltaShares);
    } else {
        node->shares += deltaShares;
        if (node->shares <= 0) {
            deleteNode(root, symbol); // Remove node if no shares are left
        }
    }
}


    void sellStock(Stock& stock, int shares) {
    if (holdings(stock.getSymbol()) >= shares) {
        stock.sellShares(shares);
        updateShares(root, stock.getSymbol(), -shares); // Update the number of shares
        cash += shares * stock.getCurrentPrice();

        // Record transaction
        string transaction = "Sold " + to_string(shares) + " shares of " +
            stock.getSymbol() + " at $" + to_string(stock.getCurrentPrice());
        transactionHistory.push_back(transaction);
    } else {
        throw runtime_error("Not enough shares in portfolio");
    }
}


    int holdings(const string& symbol) const {
        TreeNode* current = root;
        while (current != nullptr) {
            if (symbol < current->symbol) {
                current = current->left;
            } else if (symbol > current->symbol) {
                current = current->right;
            } else {
                return current->shares;
            }
        }
        return 0; // Symbol not found
    }

    void displayPortfolioSummary() const {
        cout << "\n=== Portfolio Summary ===\n";
        cout << "Cash: $" << fixed << setprecision(2) << cash << "\n\n";
        cout << "Holdings:\n";
        display(root);
    }

    void displayTransactionHistory() const {
        cout << "\n=== Transaction History ===\n";
        for (const auto& transaction : transactionHistory) {
            cout << transaction << "\n";
        }
    }
};

// Max-Heap for Top N Stocks
class MaxHeap {
private:
    vector<pair<string, double>> heap;

    void heapifyUp(int index) {
        while (index > 0) {
            int parent = (index - 1) / 2;
            if (heap[index].second > heap[parent].second) {
                swap(heap[index], heap[parent]);
                index = parent;
            } else {
                break;
            }
        }
    }

    void heapifyDown(int index) {
        int size = heap.size();
        while (index < size) {
            int left = 2 * index + 1;
            int right = 2 * index + 2;
            int largest = index;

            if (left < size && heap[left].second > heap[largest].second) {
                largest = left;
            }
            if (right < size && heap[right].second > heap[largest].second) {
                largest = right;
            }
            if (largest != index) {
                swap(heap[index], heap[largest]);
                index = largest;
            } else {
                break;
            }
        }
    }

public:
    void insert(const string& symbol, double price) {
        heap.emplace_back(symbol, price);
        heapifyUp(heap.size() - 1);
    }

    void removeMax() {
        if (heap.empty()) return;
        heap[0] = heap.back();
        heap.pop_back();
        heapifyDown(0);
    }

    void displayTopN(int N) const {
        cout << "\n=== Top " << N << " Stocks ===\n";
        for (int i = 0; i < min(N, (int)heap.size()); ++i) {
            cout << heap[i].first << ": $" << fixed << setprecision(2) << heap[i].second << "\n";
        }
    }
};

// Graph for Market Relationships
class Graph {
private:
    map<string, vector<string>> adjacencyList;

public:
    void addEdge(const string& stock1, const string& stock2) {
        adjacencyList[stock1].push_back(stock2);
        adjacencyList[stock2].push_back(stock1); // Assuming undirected graph
    }

    void displayRelationships() const {
        cout << "\n=== Stock Relationships ===\n";
        for (const auto& pair : adjacencyList) {
            cout << pair.first << " is related to: ";
            for (const auto& relatedStock : pair.second) {
                cout << relatedStock << " ";
            }
            cout << endl;
        }
    }
};

// StockMarket Class
class StockMarket {
private:
    map<string, Stock> stocks;
    MaxHeap topStocks;
    Graph stockGraph;

public:
    StockMarket() {
        stocks.emplace("AAPL", Stock("AAPL", 150.0, 1000));
        stocks.emplace("GOOG", Stock("GOOG", 2500.0, 500));
        stocks.emplace("MSFT", Stock("MSFT", 200.0, 2000));
        stocks.emplace("AMZN", Stock("AMZN", 3000.0, 1500));
        stocks.emplace("TSLA", Stock("TSLA", 700.0, 800));
    }

    Stock& getStock(const string& symbol) {
        if (stocks.find(symbol) != stocks.end()) {
            return stocks[symbol];
        } else {
            throw runtime_error("Stock not found");
        }
    }

    void updateMarket() {
        for (auto& pair : stocks) {
            pair.second.updatePrice();
            topStocks.insert(pair.first, pair.second.getCurrentPrice());
        }
    }

    void displayMarketStatus() const {
        cout << "\n=== Market Status ===\n";
        for (const auto& pair : stocks) {
            cout << pair.first << ": $" << fixed << setprecision(2) << pair.second.getCurrentPrice() << "\n";
        }
    }

    void displayMarketNews() const {
        cout << "\n=== Market News ===\n";
        cout << "No news available.\n";
    }

    void searchStock(const string& symbol) const {
        if (stocks.find(symbol) != stocks.end()) {
            cout << "Stock found: " << symbol << " - $" << stocks.at(symbol).getCurrentPrice() << "\n";
        } else {
            cout << "Stock not found: " << symbol << "\n";
        }
    }

    void displayTopStocks(int N) const {
        topStocks.displayTopN(N);
    }

    void addStockRelationship(const string& stock1, const string& stock2) {
        stockGraph.addEdge(stock1, stock2);
    }

    void displayStockRelationships() const {
        stockGraph.displayRelationships();
    }
};

// PortfolioManager Class
class PortfolioManager {
private:
    map<string, Portfolio> portfolios;
    string currentPortfolioName;

public:
    void createPortfolio(const string& name, double initialCash = 10000.0) {
        portfolios.emplace(name, Portfolio(initialCash));
        cout << "Portfolio '" << name << "' created.\n";
    }

    void selectPortfolio(const string& name) {
        if (portfolios.find(name) != portfolios.end()) {
            currentPortfolioName = name;
            cout << "Selected portfolio: " << name << "\n";
        } else {
            throw runtime_error("Portfolio not found");
        }
    }

    Portfolio& getCurrentPortfolio() {
        if (currentPortfolioName.empty()) {
            throw runtime_error("No portfolio selected");
        }
        return portfolios[currentPortfolioName];
    }

    void displayPortfolios() const {
        cout << "\n=== Available Portfolios ===\n";
        for (const auto& pair : portfolios) {
            cout << pair.first << "\n";
        }
    }

    bool hasPortfolios() const {
        return !portfolios.empty();
    }
};

// Main Function
int main() {
    StockMarket market;
    PortfolioManager portfolioManager;
    string choice;

    cout << "Welcome to Stock Market Simulator!\n";

    do {
        cout << "\n=== Menu ===\n";
        cout << "1. Display Market Status\n";
        cout << "2. Display Market News\n";
        cout << "3. Manage Portfolios\n";
        cout << "4. Buy Stock\n";
        cout << "5. Sell Stock\n";
        cout << "6. View Transaction History\n";
        cout << "7. View Portfolio Summary\n";
        cout << "8. Search for a Stock\n";
        cout << "9. View Stock Price History\n";
        cout << "10. Advance Time (Update Market)\n";
        cout << "11. Display Top N Stocks\n";
        cout << "12. Manage Stock Relationships\n";
        cout << "13. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        // Validate input
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a number from 1 to 13.\n";
            continue;
        }

        try {
            if (choice == "1") {
                market.displayMarketStatus();
            } else if (choice == "2") {
                market.displayMarketNews();
            } else if (choice == "3") {
                char portfolioChoice;
                cout << "\n=== Portfolio Management ===\n";
                cout << "1. Create New Portfolio\n";
                cout << "2. Select Existing Portfolio\n";
                cout << "3. Display Available Portfolios\n";
                cout << "4. Back to Main Menu\n";
                cout << "Enter your choice: ";
                cin >> portfolioChoice;

                if (cin.fail()) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Invalid input. Please enter a valid option.\n";
                    continue;
                }

                switch (portfolioChoice) {
                    case '1': {
                        string name;
                        cout << "Enter portfolio name: ";
                        cin >> name;
                        portfolioManager.createPortfolio(name);
                        break;
                    }
                    case '2': {
                        string name;
                        cout << "Enter portfolio name: ";
                        cin >> name;
                        portfolioManager.selectPortfolio(name);
                        break;
                    }
                    case '3':
                        portfolioManager.displayPortfolios();
                        break;
                    case '4':
                        break;
                    default:
                        cout << "Invalid choice. Please try again.\n";
                }
            } else if (choice == "4") {
                if (!portfolioManager.hasPortfolios()) {
                    cout << "No portfolios available. Please create a portfolio first.\n";
                    continue;
                }

                string symbol;
                int shares;
                cout << "Enter stock symbol: ";
                cin >> symbol;
                cout << "Enter number of shares: ";
                cin >> shares;

                try {
                    Stock& stock = market.getStock(symbol);
                    portfolioManager.getCurrentPortfolio().buyStock(stock, shares);
                    cout << "Stock purchased successfully.\n";
                } catch (const runtime_error& e) {
                    cout << "Error: " << e.what() << "\n";
                }
            } else if (choice == "5") {
                if (!portfolioManager.hasPortfolios()) {
                    cout << "No portfolios available. Please create a portfolio first.\n";
                    continue;
                }

                string symbol;
                int shares;
                cout << "Enter stock symbol: ";
                cin >> symbol;
                cout << "Enter number of shares: ";
                cin >> shares;

                try {
                    Stock& stock = market.getStock(symbol);
                    portfolioManager.getCurrentPortfolio().sellStock(stock, shares);
                    cout << "Stock sold successfully.\n";
                } catch (const runtime_error& e) {
                    cout << "Error: " << e.what() << "\n";
                }
            } else if (choice == "6") {
                if (!portfolioManager.hasPortfolios()) {
                    cout << "No portfolios available. Please create a portfolio first.\n";
                    continue;
                }

                portfolioManager.getCurrentPortfolio().displayTransactionHistory();
            } else if (choice == "7") {
                if (!portfolioManager.hasPortfolios()) {
                    cout << "No portfolios available. Please create a portfolio first.\n";
                    continue;
                }

                portfolioManager.getCurrentPortfolio().displayPortfolioSummary();
            } else if (choice == "8") {
                string symbol;
                cout << "Enter stock symbol: ";
                cin >> symbol;
                market.searchStock(symbol);
            } else if (choice == "9") {
                string symbol;
                cout << "Enter stock symbol: ";
                cin >> symbol;

                try {
                    Stock& stock = market.getStock(symbol);
                    stock.displayPriceHistory();
                } catch (const runtime_error& e) {
                    cout << "Error: " << e.what() << "\n";
                }
            } else if (choice == "10") {
                market.updateMarket();
                cout << "Market updated!\n";
            } else if (choice == "11") {
                int N;
                cout << "Enter the number of top stocks to display: ";
                cin >> N;
                market.displayTopStocks(N);
            } else if (choice == "12") {
                char relationshipChoice;
                cout << "\n=== Manage Stock Relationships ===\n";
                cout << "1. Add Relationship\n";
                cout << "2. Display Relationships\n";
                cout << "3. Back to Main Menu\n";
                cout << "Enter your choice: ";
                cin >> relationshipChoice;

                if (cin.fail()) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Invalid input. Please enter a valid option.\n";
                    continue;
                }

                switch (relationshipChoice) {
                    case '1': {
                        string stock1, stock2;
                        cout << "Enter the first stock symbol: ";
                        cin >> stock1;
                        cout << "Enter the second stock symbol: ";
                        cin >> stock2;
                        market.addStockRelationship(stock1, stock2);
                        cout << "Relationship added successfully.\n";
                        break;
                    }
                    case '2':
                        market.displayStockRelationships();
                        break;
                    case '3':
                        break;
                    default:
                        cout << "Invalid choice. Please try again.\n";
                }
            } else if (choice == "13") {
                cout << "Thank you for using Stock Market Simulator!\n";
                break;
            } else {
                cout << "Invalid choice. Please try again.\n";
            }
        } catch (const runtime_error& e) {
            cout << "Error: " << e.what() << "\n";
        }
    } while (choice != "13");

    return 0;
}