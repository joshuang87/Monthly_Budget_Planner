#include <iostream>
#include <vector>
#include <map>
#include <iomanip>
#include <ctime>
#include <string>
#include <limits>
using namespace std;

// Structure and global variables
struct Currency {
    string symbol;
    double rate;
};

map<string, Currency> currencies = {
    {"MYR", {"RM", 1.0}},          //malaysia ringgit
    {"USD", {"$", 0.21}},          //Us Dollar
    {"EUR", {"€", 0.19}}           // Euro
};

struct Month {
    // Value of month
    int value;
    int year;
    double budget;
    
    // Overloaded != operator for Month struct 
    bool operator!=(const Month& other) const {
        return value != other.value || year != other.year || budget != other.budget;
    }
};

string currentCurrency = "MYR";
double budget = 0;  // Added budget variable

// Function declarations
void showLine();
void showDate();
void showMenu();
void waitEnter();
void changeCurrency();
void setBudget();
void editCategories(vector<string>& cats, vector<vector<double>>& expenses);
void appSettings(vector<string>& cats, vector<vector<double>>& expenses);
void addExpense(vector<vector<double>>& expenses, const vector<string>& cats);
void showSummary(const vector<string>& cats, const vector<vector<double>>& expenses);

void showLine() {
    cout << "----------------------------------------" << endl;
}

void showDate() {
    time_t t = time(0);
    tm* now = localtime(&t);
    cout << "Current Date: ";
    cout << (now->tm_year + 1900) << '-';
    cout << (now->tm_mon + 1) << '-';
    cout << now->tm_mday << endl;
}

void showMenu() {
    system("cls");
    cout << endl << "Monthly Budget Planner" << endl;
    showLine();
    cout << "Current ";
    showDate();
    showLine();
    cout << "1. Set Budget" << endl;
    cout << "2. Make Expenses Record" << endl;
    cout << "3. App Setting" << endl;
    cout << "4. Exit" << endl;
    showLine();
    cout << "Enter your choice: ";
}

void waitEnter() {
    cout << endl << "Press Enter to continue...";
    cin.ignore();
    cin.get();
}

void setBudget() {
    system("cls");
    cout << endl << "Set Budget" << endl;
    showLine();
    
    Currency curr = currencies[currentCurrency];
    cout << "Enter budget amount " << curr.symbol << " ";
    cin >> budget;
    
    while (budget <= 0) {
        cout << "Invalid! Enter again " << curr.symbol << " ";
        cin >> budget;
    }
    
    cout << "Budget set to " << curr.symbol << fixed << setprecision(2) << budget << endl;
    waitEnter();
}

void changeCurrency() {
    system("cls");
    cout << endl << "Change Currency" << endl;
    showLine();
    
    cout << "Available currencies:" << endl;
    for (const auto& curr : currencies) {
        cout << curr.first << " (" << curr.second.symbol << ")" << endl;
    }
    
    cout << endl << "Enter currency code (MYR/USD/EUR): ";
    string newCurrency;
    cin >> newCurrency;
    
    if (currencies.find(newCurrency) != currencies.end()) {
        currentCurrency = newCurrency;
        cout << "Currency changed to " << newCurrency << endl;
    } else {
        cout << "Invalid currency code" << endl;
    }
    waitEnter();
}

void editCategories(vector<string>& cats, vector<vector<double>>& expenses) {
    while (true) {
        system("cls");
        cout << endl << "Edit Categories" << endl;
        showLine();
        
        cout << "Current Categories:" << endl;
        for (int i = 0; i < cats.size(); i++) {
            cout << i + 1 << ". " << cats[i] << endl;
        }
        
        showLine();
        cout << "1. Add category" << endl;
        cout << "2. Remove category" << endl;
        cout << "3. Back" << endl;
        showLine();
        cout << "Choice: ";
        
        int choice;
        cin >> choice;
        cin.ignore();

        if (choice == 1) {
            string newCat;
            cout << "Enter new category name: ";
            getline(cin, newCat);
            cats.push_back(newCat);
            expenses.push_back(vector<double>());
            cout << "Category added!" << endl;
        }
        else if (choice == 2) {
            if (cats.size() > 1) {
                int num;
                cout << "Enter category number to remove: ";
                cin >> num;
                if (num >= 1 && num <= cats.size()) {
                    cats.erase(cats.begin() + num - 1);
                    expenses.erase(expenses.begin() + num - 1);
                    cout << "Category removed!" << endl;
                }
            } else {
                cout << "Cannot remove - need at least one category!" << endl;
            }
        }
        else if (choice == 3) {
            break;
        }
        waitEnter();
    }
}

void appSettings(vector<string>& cats, vector<vector<double>>& expenses) {
    while(true) {
        system("cls");
        cout << endl << "App Settings" << endl;
        showLine();
        cout << "1. Edit Categories" << endl;
        cout << "2. Change Currency" << endl;
        cout << "3. Back" << endl;
        showLine();
        cout << "Choice: ";
        
        int choice;
        cin >> choice;
        
        if (choice == 1) {
            editCategories(cats, expenses);
        }
        else if (choice == 2) {
            changeCurrency();
        }
        else if (choice == 3) {
            break;
        }
    }
}

void addExpense(vector<vector<double>>& expenses, const vector<string>& cats) {
    char addMore;
    do {
        system("cls");
        cout << endl << "Record Expenses" << endl;
        showLine();
        
        cout << "Categories:" << endl;
        for (int i = 0; i < cats.size(); i++) {
            cout << i + 1 << ". " << cats[i] << endl;
        }
        showLine();
        
        int catNum;
        cout << "Enter category number: ";
        cin >> catNum;
        
        while (catNum < 1 || catNum > cats.size()) {
            cout << "Invalid! Enter again: ";
            cin >> catNum;
        }

        Currency curr = currencies[currentCurrency];
        double amount;
        cout << "Enter amount " << curr.symbol << " ";
        cin >> amount;
        
        while (amount < 0) {
            cout << "Invalid! Enter again " << curr.symbol << " ";
            cin >> amount;
        }

        // Calculate total expenses including new amount
        double totalExpense = amount / curr.rate;
        for (const auto& catExp : expenses) {
            for (double exp : catExp) {
                totalExpense += exp;
            }
        }

        // Budget warning check
        if (budget > 0 && (totalExpense * curr.rate) > budget) {
            cout << "\nWARNING: This expense will exceed your budget of "
                 << curr.symbol << fixed << setprecision(2) << budget << "!\n";
            cout << "Do you want to continue? (y/n): ";
            char confirm;
            cin >> confirm;
            if (confirm != 'y' && confirm != 'Y') {
                continue;
            }
        }

        expenses[catNum - 1].push_back(amount / curr.rate);
        
        cout << endl << "Add another? (y/n): ";
        cin >> addMore;
    } while (addMore == 'y' || addMore == 'Y');
}

void showSummary(const vector<string>& cats, const vector<vector<double>>& expenses) {
    system("cls");
    cout << endl << "Expenses Summary" << endl;
    showLine();
    
    Currency curr = currencies[currentCurrency];
    double total = 0;

    for (int i = 0; i < cats.size(); i++) {
        double catTotal = 0;
        for (double exp : expenses[i]) {
            catTotal += exp;
        }
        total += catTotal;
        
        cout << cats[i] << ": " << curr.symbol 
             << setprecision(2) << (catTotal * curr.rate) << endl;
    }
    
    showLine();
    cout << "Total: " << curr.symbol 
         << setprecision(2) << (total * curr.rate) << endl;
    
    if (budget > 0) {
        cout << "Budget: " << curr.symbol << setprecision(2) << budget << endl;
        double remaining = budget - (total * curr.rate);
        cout << "Remaining: " << curr.symbol << setprecision(2) << remaining << endl;
    }
}

int main() {
    vector<string> categories = {"Food", "Beverage", "Clothes"};
    vector<vector<double>> expenses(categories.size());
    int choice;
    
    do {
        showMenu();
        cin >> choice;
        
        if (choice == 1) {
            setBudget();
        }
        else if (choice == 2) {
            addExpense(expenses, categories);
        }
        else if (choice == 3) {
            appSettings(categories, expenses);
        }
        else if (choice == 4) {
            showSummary(categories, expenses);
            cout << endl << "Goodbye!" << endl;
        }
        
    } while (choice != 4);
    
    return 0;
}