#include <iostream>
#include <ctime>
#include <limits>
#include <vector>
#include <map>
#include <iomanip>
#include <stdlib.h>

using namespace std;

// Currency structure and global variables
struct Currency {
    string symbol;
    double rate;
};

map<string, Currency> currencies = {
    {"MYR", {"RM", 1.0}},
    {"USD", {"$", 0.21}},
    {"EUR", {"€", 0.19}}
};
string currentCurrency = "MYR";

void displayBorder();
void displayTitle(const string& title);
void displayMainMenu();
void displaySettingsMenu();

void CurrentDate();
void userinput_expenses(vector<vector<double>> &categoryexpenses, const vector<string> &category);
void editExpCat(vector<string> &categories, vector<vector<double>> &categoryexpenses);

void currencySettings();
void showSummary(const vector<string>& categories, const vector<vector<double>>& expenses);
void appSettings(vector<string> &categories, vector<vector<double>> &categoryexpenses);
void pressEnterToContinue();

// Utility functions
void displayBorder() {
    cout << "\n+----------------------------------------+\n";
}
void displayTitle(const string& title) {
    displayBorder();
    cout << setfill(' ') << setw(20) << title << endl;
    displayBorder();
}

void displayMainMenu() {
    system("cls"); // Windows: use "cls", Linux/Mac: use "clear"
    displayTitle("Monthly Budget Planner");
    cout << "\nCurrent ";
    CurrentDate();
    cout << "\nPlease select an option:\n";
    displayBorder();
    cout << "  1. Set Budget\n"
         << "  2. Make Expenses Record\n"
         << "  3. App Setting\n"
         << "  4. Exit\n";
    displayBorder();
    cout << "Enter your choice: ";
}

void pressEnterToContinue() {
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

int main() {
    int choice;
    vector<string> category = {"Food", "Beverage", "Clothes"};
    vector<vector<double>> categoryexpenses(category.size());

    do {
        displayMainMenu();

         while (!(cin >> choice)) {

            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a number between 1 and 4: ";

        }

        while (choice < 1 || choice > 4) {

            cout << "Invalid choice. Please enter a number between 1 and 4: ";

            while (!(cin >> choice)) {

                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input. Please enter a number between 1 and 4: ";

            }

        }

        switch (choice){
            case 1:
            cout<<"havent"<<endl;
            pressEnterToContinue();
            break;

            case 2:
            userinput_expenses(categoryexpenses, category);
            break;

            case 3:
           appSettings(category, categoryexpenses);
            break;

            case 4:
            showSummary(category, categoryexpenses);
            cout<<"Bye!"<<endl;
            break;
        }

    } while (choice != 4);
    
    return 0;
}

void CurrentDate() {

    time_t t = time(0);
    struct tm * now = localtime(&t);

    cout << "Current Date: "
         << (now->tm_year + 1900)
         << '-'
         << (now->tm_mon + 1)
         << '-'
         << now->tm_mday
         << endl;

}

void appSettings(vector<string> &categories, vector<vector<double>> &categoryexpenses) {
    while(true) {
        system("cls");
        displayTitle("App Settings");
        cout << "\nPlease select an option:\n";
        displayBorder();
        cout << "  1. Edit Categories\n"
             << "  2. Currency Settings\n"
             << "  3. Back to Main Menu\n";
        displayBorder();
        cout << "Enter choice: ";
        
        int settingChoice;
        while (!(cin >> settingChoice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a number between 1 and 3: ";
        }
        
        while (settingChoice < 1 || settingChoice > 3) {
            cout << "Invalid choice. Please enter a number between 1 and 3: ";
            while (!(cin >> settingChoice)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input. Please enter a number between 1 and 3: ";
            }
        }

        switch(settingChoice) {
            case 1:
                editExpCat(categories, categoryexpenses);
                break;
            case 2:
                currencySettings();
                break;
            case 3:
                return;
            default:
                cout << "Invalid choice\n";
                pressEnterToContinue();
        }
    }
}

void userinput_expenses(vector<vector<double>> &catExpenses, const vector<string> &categories) {
    double expense;
    int catChoice;
    char option;

    while (true) {
        system("cls");
        displayTitle("Record Expenses");
        cout << "Choose a category: " << endl;
        for (int i = 0; i < categories.size(); i++) {
            cout << i + 1 << ". " << categories[i] << endl;
        }
        displayBorder();

        cout << "Enter a number to choose the category: ";
        while (!(cin>>catChoice) || catChoice < 1 || catChoice > categories.size()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid choice. Please enter a valid category number: ";

        }

        cout<<"\nSelected  category: "<<categories[catChoice-1]<<endl;
        cout<<"Do you confirm want proceed to this category? (y to confirm, n to choose again): ";
        while(true){
            cin>>option;
            if(option=='y'||option=='Y'){
                break;
            }else if (option=='n'||option=='N'){
                cout<<"Go back to the category selection menu\n";
                goto categorySelection;
            }else{
                cout<<"Invalid input. Please enter (y/n)";
            }
        }


        cout << "Enter expense amount: ";

        while (!(cin>>expense)||expense<0) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input. Please enter a valid number of expenses: ";
        }


        catExpenses[catChoice - 1].push_back(expense);


        while (true){
            cout << "\nAdd another expenses? (y/n): ";
            cin >> option;

            if (cin.fail() ||(option!='y'&&option!='Y'&&option!='n'&&option!='N')){
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout<<"Invalid input. Please enter (y/n)."<<endl;
                continue;
            }

            if(option=='y'||option=='Y'){
                break;
            }else if(option=='n'||option=='N'){

                displayTitle("Current Expenses Record");
                Currency curr = currencies[currentCurrency];
                for(int i=0; i<categories.size();i++) {
                    if(catExpenses[i].size() > 0) {

                       cout<<"\n Current Expenses Record\n";
                       displayBorder();
                       for(int j=0; j<catExpenses[i].size();j++){
                       
                            double converted = catExpenses[i][j] * curr.rate;
                            cout << "  Expense " << j+1 << ": " << curr.symbol 
                                 << fixed << setprecision(2) << converted << endl;
                        }
                    }
                }
                pressEnterToContinue();
                return;
            } else {
                cout<<"Invalid input. Please enter (y/n): ";
            }
        }    
    categorySelection:
        continue;

    }

}

void editExpCat(vector<string> &categories, vector<vector<double>> &categoryexpenses) {
    char choice;

    while (true) {
        system("cls");
        displayTitle("Category Management");
        cout << "\nPlease select an option:\n";
        displayBorder();

        cout << "\nCategory Management Menu:" << endl;
        cout << "1. Add a new category" << endl;
        cout << "2. Remove an existing category" << endl;
        cout << "3. View current categories" << endl;
        cout << "4. Back" << endl;
        
        displayBorder();
        cout << "Enter your choice: ";

        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); //prevent input buffer issues when use getline() next

        switch (choice) {
            case '1': { // Add new category

                string newCat;
                cout << "Enter the name of the new category: ";
                getline(cin, newCat);

                categories.push_back(newCat);
                categoryexpenses.push_back(vector<double>()); // Add corresponding empty expense vector
                cout << "Category '" << newCat << "' added successfully!" << endl;
                pressEnterToContinue();
                break;
            }

            case '2': { // Remove category

                if (categories.empty()) {
                    cout << "No categories to remove." << endl;
                    pressEnterToContinue();
                    break;
                }
                
                displayTitle("Remove Category");
                cout << "\nCurrent Categories:\n";
                displayBorder();
                for (int i = 0; i < categories.size(); i++) {
                    cout << i + 1 << ". " << categories[i] << endl;
                }
                displayBorder();

                int removeId;
                cout << "Enter the number of the category to remove: ";
                cin >> removeId;

                // Validate input
                if (cin.fail() || removeId < 1 || removeId > categories.size()) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Invalid category number." << endl;
                    pressEnterToContinue();
                    break;
                }

                // Remove category
                string removeCat = categories[removeId - 1];
                categories.erase(categories.begin() + (removeId - 1)); //point to the first element in vector + the index
                categoryexpenses.erase(categoryexpenses.begin() + (removeId - 1)); //categoryexpenses as well
                cout << "Category '" << removeCat << "' removed successfully!" << endl;
                pressEnterToContinue();
                break;
            }

            case '3': {
                // View all categories
                if (categories.empty()) {
                    cout << "No categories exist." << endl;
                } else {
                    cout << "\nCurrent Categories:" << endl;
                    for (int i = 0; i < categories.size(); i++) {
                        cout << i + 1 << ". " << categories[i] << endl;
                    }
                }
                pressEnterToContinue();
                break;
            }

            case '4':
                // Exit
                return;

            default:
                cout << "Invalid number" << endl;
                pressEnterToContinue();
        }
    }
}

void currencySettings() {
    while (true) {
        system("cls");
        displayTitle("Currency Settings");
        cout << "\nPlease select an option:\n";
        displayBorder();
        cout << "  1. View available currencies\n"
             << "  2. Change currency\n"
             << "  3. Back\n";
        displayBorder();
        cout << "Choice: ";
        
        int choice;
        cin >> choice;
        
        switch (choice) {
            case 1: {
                displayTitle("Available Currencies");
                for (const auto& curr : currencies) {
                    cout << "  " << curr.first << " (" << curr.second.symbol << ")\n";
                }
                pressEnterToContinue();
                break;
            }
            
            case 2: {
                displayBorder();
                cout << "Enter currency code (";
                for (const auto& curr : currencies) {
                    cout << curr.first << "/";
                }
                cout << "\b): ";
                
                string newCurrency;
                cin >> newCurrency;
                
                if (currencies.find(newCurrency) != currencies.end()) {
                    currentCurrency = newCurrency;
                    cout << "\nCurrency changed to " << newCurrency;
                } else {
                    cout << "\nInvalid currency code";
                }
                pressEnterToContinue();
                break;
            }
            
            case 3:
                return;
                
            default:
                cout << "\nInvalid choice";
                pressEnterToContinue();
        }
    }
}

void showSummary(const vector<string>& categories, const vector<vector<double>>& expenses) {
    system("cls");
    displayTitle("Expenses Summary");
    
    Currency curr = currencies[currentCurrency];
    double total = 0;
    
    cout << "\nCategory Breakdown:\n";
    displayBorder();
    
    for (size_t i = 0; i < categories.size(); i++) {
        double catTotal = 0;
        for (double expense : expenses[i]) {
            catTotal += expense;
        }
        total += catTotal;
        
        double converted = catTotal * curr.rate;
        cout << setw(15) << left << categories[i] << ": " 
             << curr.symbol << fixed << setprecision(2) 
             << setw(10) << right << converted << "\n";
    }
    
    displayBorder();
    cout << setw(15) << left << "Total" << ": " 
         << curr.symbol << fixed << setprecision(2) 
         << setw(10) << right << (total * curr.rate) << "\n";
    displayBorder();
}