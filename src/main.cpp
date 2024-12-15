#include <iostream>
#include <ctime>
#include <limits>
#include <vector>
#include <map>
#include <iomanip>

using namespace std;
// Currency structure and global variables
struct Currency {
    string symbol;
    double rate;
};

map<string, Currency> currencies = {
    {"MYR", {"RM", 1.0}},
    {"USD", {"$", 0.21}},
};
string currentCurrency = "MYR";

void CurrentDate();
void userinput_expenses(vector<vector<double>> &categoryexpenses, const vector<string> &category);
void editExpCat(vector<string> &categories, vector<vector<double>> &categoryexpenses);

void currencySettings();
void showSummary(const vector<string>& categories, const vector<vector<double>>& expenses);
void appSettings(vector<string> &categories, vector<vector<double>> &categoryexpenses);

int main() {

    int choice;
    vector<string> category = {"Food", "Beverage", "Clothes"};
    vector<vector<double>> categoryexpenses(category.size());

    do {

        CurrentDate();

        cout << "Monthly Budget Planner\n"
             << "1. Set Budget\n"
             << "2. Make Expenses Record\n"
             << "3. App Setting\n"
             << "4. Exit\n"
             << "Enter your choice: ";

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
 
void appSettings(vector<string> &categories, vector<vector<double>> &categoryexpenses) {
    while(true) {
        cout << "\n=== App Settings ===\n"
             << "1. Edit Categories\n"
             << "2. Currency Settings\n"
             << "3. Back to Main Menu\n"
             << "Enter choice: ";
        
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
        }
    }
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

void userinput_expenses(vector<vector<double>> &catExpenses, const vector<string> &categories) {
    double expense;
    int catChoice;
    char option;

    while (true) {

        cout << "Choose a category: " << endl;
        for (int i = 0; i < categories.size(); i++) {
            cout << i + 1 << ". " << categories[i] << endl;
        }


        cout << "Enter a number to choose the category: ";
        //
        while (!(cin>>catChoice) || catChoice < 1 || catChoice > categories.size()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid choice. Please enter a valid category number: ";

        }

        cout<<"Your category: "<<categories[catChoice-1]<<endl;
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


        cout << "Enter expense: ";

        while (!(cin>>expense)||expense<0) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input. Please enter a valid number of expenses: ";
        }


        catExpenses[catChoice - 1].push_back(expense);


        while (true){
            cout << "Do you want to continue to add expenses? (y/n): ";
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
                cout<<"\nExpenses Record\n";
                for(int i=0; i<categories.size();i++){
                    cout<<"Category: "<<categories[i]<<"\n";
                    for(int j=0; j<catExpenses[i].size();j++){
                        cout<<"Expense " <<j+1<<" :RM " <<catExpenses[i][j]<<endl;
                    }
                }
                return;
            }else{
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

        cout << "\nCategory Management Menu:" << endl;
        cout << "1. Add a new category" << endl;
        cout << "2. Remove an existing category" << endl;
        cout << "3. View current categories" << endl;
        cout << "4. Exit" << endl;
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
                break;
            }

            case '2': { // Remove category

                if (categories.empty()) {
                    cout << "No categories to remove." << endl;
                    break;
                }

                cout << "\nCurrent Categories:" << endl;
                for (int i = 0; i < categories.size(); i++) {
                    cout << i + 1 << ". " << categories[i] << endl;
                }

                int removeId;
                cout << "Enter the number of the category to remove: ";
                cin >> removeId;

                // Validate input
                if (cin.fail() || removeId < 1 || removeId > categories.size()) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Invalid category number." << endl;
                    break;
                }

                // Remove category
                string removeCat = categories[removeId - 1];
                categories.erase(categories.begin() + (removeId - 1)); //point to the first element in vector + the index
                categoryexpenses.erase(categoryexpenses.begin() + (removeId - 1)); //categoryexpenses as well
                cout << "Category '" << removeCat << "' removed successfully!" << endl;
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
                break;
            }

            case '4':
                // Exit
                return;

            default:
                cout << "Invalid number" << endl;
        }
    }
}

void currencySettings() {
    while (true) {
        cout << "\n=== Currency Settings ===\n";
        cout << "1. View available currencies\n";
        cout << "2. Change currency\n";
        cout << "3. Back\n";
        cout << "Choice: ";
        
        int choice;
        cin >> choice;
        
        switch (choice) {
            case 1: {
                cout << "\nAvailable Currencies:\n";
                for (const auto& curr : currencies) {
                    cout << curr.first << " (" << curr.second.symbol << ")\n";
                }
                break;
            }
            
            case 2: {
                string newCurrency;
                cout << "Enter currency code (MYR/USD/EUR): ";
                cin >> newCurrency;
                
                if (currencies.find(newCurrency) != currencies.end()) {
                    currentCurrency = newCurrency;
                    cout << "Currency changed to " << newCurrency << "\n";
                } else {
                    cout << "Invalid currency code\n";
                }
                break;
            }
            
            case 3:
                return;
                
            default:
                cout << "Invalid choice\n";
        }
    }
}

void showSummary(const vector<string>& categories, const vector<vector<double>>& expenses) {
    cout << "\n=== Expenses Summary ===\n";
    
    Currency curr = currencies[currentCurrency];
    double total = 0;
    
    for (size_t i = 0; i < categories.size(); i++) {
        double catTotal = 0;
        for (double expense : expenses[i]) {
            catTotal += expense;
        }
        total += catTotal;
        
        double converted = catTotal * curr.rate;
        cout << categories[i] << ": " << curr.symbol 
             << fixed << setprecision(2) << converted << "\n";
    }
    
    cout << "\nTotal Expenses: " << curr.symbol 
         << fixed << setprecision(2) << (total * curr.rate) << "\n";
}