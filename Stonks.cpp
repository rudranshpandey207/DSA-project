#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <iomanip>
#include <random>
#include <stdexcept>

using namespace std;

class Stock {
private:
    string symbol;
    double currentPrice;
    int availableShares;
    vector<double> priceHistory;

public:
    // Default constructor
    Stock() : symbol(""), currentPrice(0.0), availableShares(0) {}

    // Parameterized constructor
    Stock(string sym, double price, int shares) : 
        symbol(sym), currentPrice(price), availableShares(shares) {
        priceHistory.push_back(price);
    }

    // Getters
    string getSymbol() const { return symbol; }
    double getCurrentPrice() const { return currentPrice; }
    int getAvailableShares() const { return availableShares; }
    
    // Update price with some random fluctuation
    void updatePrice() {
        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<> dis(-2.0, 2.0);
        
        double change = dis(gen);
        currentPrice = max(0.01, currentPrice + change);
        priceHistory.push_back(currentPrice);
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
};

class Portfolio {
private:
    map<string, int> holdings;  // Stock symbol -> number of shares
    double cash;
    vector<string> transactionHistory;

public:
    Portfolio(double initialCash = 10000.0) : cash(initialCash) {}

    void buyStock(Stock& stock, int shares) {
        double cost = shares * stock.getCurrentPrice();
        if (cost <= cash) {
            stock.buyShares(shares);
            holdings[stock.getSymbol()] += shares;
            cash -= cost;
            
            // Record transaction
            string transaction = "Bought " + to_string(shares) + " shares of " + 
                               stock.getSymbol() + " at $" + to_string(stock.getCurrentPrice());
            transactionHistory.push_back(transaction);
        } else {
            throw runtime_error("Insufficient funds");
        }
    }

    void sellStock(Stock& stock, int shares) {
        if (holdings[stock.getSymbol()] >= shares) {
            stock.sellShares(shares);
            holdings[stock.getSymbol()] -= shares;
            cash += shares * stock.getCurrentPrice();
            
            // Record transaction
            string transaction = "Sold " + to_string(shares) + " shares of " + 
                               stock.getSymbol() + " at $" + to_string(stock.getCurrentPrice());
            transactionHistory.push_back(transaction);
        } else {
            throw runtime_error("Not enough shares in portfolio");
        }
    }

    void displayPortfolio() const {
        cout << "\n=== Portfolio Summary ===\n";
        cout << "Cash: $" << fixed << setprecision(2) << cash << "\n\n";
        cout << "Holdings:\n";
        for (const auto& holding : holdings) {
            if (holding.second > 0) {
                cout << holding.first << ": " << holding.second << " shares\n";
            }
        }
    }

    void displayTransactionHistory() const {
        cout << "\n=== Transaction History ===\n";
        for (const auto& transaction : transactionHistory) {
            cout << transaction << "\n";
        }
    }
};

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

class StockMarket {
private:
    map<string, Stock> stocks;

public:
    StockMarket() {
        // Initialize some sample stocks
        stocks.emplace("AAPL", Stock("AAPL", 150.0, 1000));
        stocks.emplace("GOOG", Stock("GOOG", 2500.0, 500));
        stocks.emplace("MSFT", Stock("MSFT", 200.0, 2000));
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
};

int main() {
    StockMarket market;
    PortfolioManager portfolioManager;
    char choice;

    cout << "Welcome to Stock Market Simulator!\n";

    do {
        cout << "\n=== Menu ===\n";
        cout << "1. Display Market Status\n";
        cout << "2. Display Market News\n";
        cout << "3. Manage Portfolios\n";
        cout << "4. Buy Stock\n";
        cout << "5. Sell Stock\n";
        cout << "6. View Transaction History\n";
        cout << "7. Advance Time (Update Market)\n";
        cout << "8. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        try {
            switch (choice) {
                case '1':
                    market.displayMarketStatus();
                    break;
                
                case '2':
                    market.displayMarketNews();
                    break;
                
                case '3': {
                    char portfolioChoice;
                    cout << "\n=== Portfolio Management ===\n";
                    cout << "1. Create New Portfolio\n";
                    cout << "2. Select Existing Portfolio\n";
                    cout << "3. Display Available Portfolios\n";
                    cout << "4. Back to Main Menu\n";
                    cout << "Enter your choice: ";
                    cin >> portfolioChoice;

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
                    break;
                }
                
                case '4': {
                    if (!portfolioManager.hasPortfolios()) {
                        cout << "No portfolios created. Please create a portfolio first.\n";
                        break;
                    }
                    
                    string symbol;
                    int shares;
                    cout << "Enter stock symbol: ";
                    cin >> symbol;
                    cout << "Enter number of shares: ";
                    cin >> shares;
                    Portfolio& currentPortfolio = portfolioManager.getCurrentPortfolio();
                    currentPortfolio.buyStock(market.getStock(symbol), shares);
                    break;
                }
                
                case '5': {
                    if (!portfolioManager.hasPortfolios()) {
                        cout << "No portfolios created. Please create a portfolio first.\n";
                        break;
                    }
                    
                    string symbol;
                    int shares;
                    cout << "Enter stock symbol: ";
                    cin >> symbol;
                    cout << "Enter number of shares: ";
                    cin >> shares;
                    Portfolio& currentPortfolio = portfolioManager.getCurrentPortfolio();
                    currentPortfolio.sellStock(market.getStock(symbol), shares);
                    break;
                }
                
                case '6': {
                    if (!portfolioManager.hasPortfolios()) {
                        cout << "No portfolios created. Please create a portfolio first.\n";
                        break;
                    }
                    
                    portfolioManager.getCurrentPortfolio().displayTransactionHistory();
                    break;
                }
                
                case '7':
                    market.updateMarket();
                    cout << "Market updated!\n";
                    break;
                
                case '8':
                    cout << "Thank you for using Stock Market Simulator!\n";
                    break;
                
                default:
                    cout << "Invalid choice. Please try again.\n";
            }
        } catch (const runtime_error& e) {
            cout << "Error: " << e.what() << "\n";
        }
    } while (choice != '8');

    return 0;
}