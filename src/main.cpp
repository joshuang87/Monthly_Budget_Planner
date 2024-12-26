#include <iostream>
#include <vector>
#include <map>
#include <iomanip>
#include <ctime>
#include <string>
#include <limits>
#include <filesystem>
#include <fstream>
#include <regex>
#include <array>

using namespace std;

// Structure and global variables
array<string, 3> default_categories = {"Food", "Beverage", "Clothes"};

struct Currency {
    string symbol;
    double rate;
};

map<string, Currency> currencies = {
    {"MYR", {"RM", 1.0}},          //malaysia ringgit
    {"USD", {"$", 0.21}},          //Us Dollar
    {"EUR", {"€", 0.19}}           // Euro
};

struct Budget {
    // amount of budget 
    double amount;
    int year;
    int month;
    
    // Overloaded != operator for Budget struct 
    bool operator!=(const Budget& other) const {
        return amount != other.amount || year != other.year || month != other.month;
    }
};

struct Expense
{
    int id;
    string category;
    double amount;
    string remarks;
    int day;
    int month;
    int year;

    // Overloaded != operator for Expense struct 
    bool operator!=(const Expense& other) const {
        return id != other.id || category != other.category || amount != other.amount || remarks != other.remarks || day != other.day || month != other.month || year != other.year;
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
void setBudget(int month, int year);
void editCategories(vector<string>& cats, vector<vector<double>>& expenses);
void appSettings(vector<string>& cats, vector<vector<double>>& expenses);
void addExpense(vector<vector<double>>& expenses, const vector<string>& cats);
void showSummary(const vector<string>& cats, const vector<vector<double>>& expenses);

/**
 * @brief Region for C++ structs data formatting functionally - converts C++ structs to JSON strings
 */
#pragma region Format Struct Data as JSON String

/**
 * @brief Convert vector of data to JSON string format
 * @tparam T The type of elements in the vector
 * @param data Vector containing the data to be converted
 * @return std::string JSON formatted string representation
 */
template <typename T>
string to_json_str(const vector<T>& data);

/**
 * @brief Template specialization to convert vector of Budget objects to JSON string
 * @param data Vector of Budget objects
 * @return std::string JSON formatted string containing month, year and amount data
 * @details Each Budget object is formatted as {"amount": value, "year": value, "month": value}
 */
template <>
string to_json_str<Budget>(const vector<Budget>& data) {
    string str_data = "[";
    for (Budget budget : data) {
        str_data += "{";
        str_data += "\"amount\": " + to_string(budget.amount) + ",";
        str_data += "\"year\": " + to_string(budget.year) + ",";
        str_data += "\"month\": " + to_string(budget.month);
        if (budget != data.back()) {
            str_data += "},";
        } else {
            str_data += "}";
        }
    }
    str_data += "]";
    return str_data;
}

/**
 * @brief Template specialization to convert vector of Expense objects to JSON string
 * @param data Vector of Expense objects
 * @return std::string JSON formatted string containing expense details
 * @details Each Expense object is formatted as 
 * {"id": value, "category": "value", "amount": value, "remarks": "value", 
 *  "day": value, "month": value, "year": value}
 */
template <>
string to_json_str<Expense>(const vector<Expense>& data) {
    string str_data = "[";
    for (Expense expense : data) {
        str_data += "{";
        str_data += "\"id\": " + to_string(expense.id) + ",";
        str_data += "\"category\": \"" + expense.category + "\",";
        str_data += "\"amount\": " + to_string(expense.amount) + ",";
        str_data += "\"remarks\": \"" + expense.remarks + "\",";
        str_data += "\"day\": " + to_string(expense.day) + ",";
        str_data += "\"month\": " + to_string(expense.month) + ",";
        str_data += "\"year\": " + to_string(expense.year);
        if (expense != data.back()) {
            str_data += "},";
        } else {
            str_data += "}";
        }
    }
    str_data += "]";
    return str_data;
}

/**
 * @brief Template specialization to convert vector of strings to JSON string
 * @param data Vector of strings
 * @return std::string JSON formatted array of strings
 * @details Output format: ["string1","string2",...,"stringN"]
 */
template <>
string to_json_str<string>(const vector<string>& data) {
    string str_data = "[";
    for (int i = 0; i < data.size(); i++) {
        str_data += "\"" + data[i] + "\"";
        if (i != data.size() - 1) {
            str_data += ",";
        }
    }
    str_data += "]";
    return str_data;
}

/**
 * @brief Convert fixed-size array of strings to JSON string
 * @tparam N Size of the array
 * @param data Array of strings
 * @return std::string JSON formatted array of strings
 * @details Output format: ["string1","string2",...,"stringN"]
 */
template <size_t N>
string to_json_str(const array<string, N>& data) {
    string str_data = "[";
    for (int i = 0; i < data.size(); i++) {
        str_data += "\"" + data[i] + "\"";
        if (i != data.size() - 1) {
            str_data += ",";
        }
    }
    str_data += "]";
    return str_data;
}

#pragma endregion Format Struct Data as JSON String

/**
 * @brief Region for JSON parsing functionality - converts JSON strings to C++ structs
 */
#pragma region JSON Parsing (JSON String -> C++ Struct)

/**
 * @brief Generic template function for parsing JSON data into vector of objects
 * @tparam T Type of object to parse into
 * @param json_str Input JSON string to be parsed
 * @return vector<T> Vector of parsed objects
 */
template <typename T>
vector<T> parse_json (const string& json_str);

/**
 * @brief Template specialization for parsing Budget JSON data
 * @param json_str JSON string containing budget data
 * @return vector<Month> Vector of budget objects
 * @details Expects JSON format:
 *          {"amount": double, "year": int, "month": int}
 */
template <>
vector<Budget> parse_json<Budget>(const string& json_str) {
    vector<Budget> budgets;
    stringstream ss(json_str);
    string line;

    while (getline(ss, line, '{')) {
        // Skip non-month entries
        if (line.find("month") == string::npos) continue;

        Budget b;
        size_t pos;

        // Parse budget amount
        pos = line.find("amount");
        b.amount = stod(line.substr(pos + 9, line.find(",", pos) - pos - 9));

        // Parse year
        pos = line.find("year");
        b.year = stoi(line.substr(pos + 6, line.find(",", pos) - pos - 6));

        // Parse month 
        pos = line.find("month");
        b.month = stoi(line.substr(pos + 8, line.find("}", pos) - pos - 8));

        budgets.push_back(b);
    }

    return budgets;
}

/**
 * @brief Template specialization for parsing Expense JSON data
 * @param json_str JSON string containing expense data
 * @return vector<Expense> Vector of Expense objects
 * @details Expects JSON format:
 *          {"id": int, "category": string, "amount": double, 
 *           "remarks": string, "day": int, "month": int, "year": int}
 */
template <>
vector<Expense> parse_json<Expense>(const string& json_str) {
    vector<Expense> expenses;
    stringstream ss(json_str);
    string line;

    while (getline(ss, line, '{')) {
        // Skip non-expense entries
        if (line.find("id") == string::npos) continue;

        Expense e;
        size_t pos;

        // Parse each field of the expense object
        pos = line.find("id");
        e.id = stoi(line.substr(pos + 4, line.find(",", pos) - pos - 4));

        pos = line.find("category");
        e.category = line.substr(pos + 11, line.find(",", pos) - pos - 12);

        pos = line.find("amount");
        e.amount = stod(line.substr(pos + 7, line.find(",", pos) - pos - 7));

        pos = line.find("remarks");
        e.remarks = line.substr(pos + 9, line.find(",", pos) - pos - 10);

        pos = line.find("day");
        e.day = stoi(line.substr(pos + 5, line.find(",", pos) - pos - 5));

        pos = line.find("month");
        e.month = stoi(line.substr(pos + 7, line.find(",", pos) - pos - 7));

        pos = line.find("year");
        e.year = stoi(line.substr(pos + 6, line.find("}", pos) - pos - 6));

        expenses.push_back(e);
    }

    return expenses;
}

/**
 * @brief Template specialization for parsing category JSON data into strings
 * @param json_str JSON string containing category data
 * @return vector<string> Vector of category strings
 * @details Expects JSON format: ["category1", "category2", ...]
 *          Only accepts alphanumeric categories matching regex: [A-Za-z0-9]+
 */
template <>
vector<string> parse_json<string>(const string& json_str) {
    vector<string> categories;
    stringstream ss(json_str);
    string line;
    // Regex for alphanumeric validation
    regex reg(R"(^[A-Za-z0-9]+$)");

    while (getline(ss, line, '\"')) {
        if (regex_match(line, reg)) {
            categories.push_back(line);
        }
    }

    return categories;
}

#pragma endregion JSON Parsing (JSON String -> C++ Struct)

/**
 * @brief Region for saving C++ data structures to JSON files
 */
#pragma region Save Data as JSON (C++ Struct -> JSON)

/**
 * @brief Generic template function for saving data to JSON file
 * @tparam T Type of the data to be saved
 * @param data Vector of objects to be saved
 */
template <typename T>
void save_as_json(const vector<T>& data);


/**
 * @brief Template specialization for saving Budget data to JSON file
 * @param data Vector of Budget objects to be saved
 * @details Saves to "data/Budget.json" using the Budget to JSON string converter
 * @see to_json_str<Budget>
 */
template <>
void save_as_json<Budget>(const vector<Budget>& data) {
    ofstream file;
    file.open("data/Budget.json");
    file << to_json_str<Budget>(data);
    file.close();
}


/**
 * @brief Template specialization for saving Expense data to JSON file
 * @param data Vector of Expense objects to be saved
 * @details Saves to "data/Expense.json" using the Expense to JSON string converter
 * @see to_json_str<Expense>
 */
template <>
void save_as_json<Expense>(const vector<Expense>& data) {
    ofstream file;
    file.open("data/Expense.json");
    file << to_json_str<Expense>(data);
    file.close();
}

/**
 * @brief Template specialization for saving category strings to JSON file
 * @param data Vector of category strings to be saved
 * @details Saves to "data/Category.json" using the string to JSON string converter
 * @see to_json_str<string>
 */
template <>
void save_as_json<string>(const vector<string>& data) {
    ofstream file;
    file.open("data/Category.json");
    file << to_json_str<string>(data);
    file.close();
}

/**
 * @brief Save fixed-size array of strings to JSON file
 * @tparam N Size of the array
 * @param data Array of strings to be saved
 * @details Saves to "data/Category.json" using the array to JSON string converter
 * @see to_json_str<N>
 */
template <size_t N>
void save_as_json(const array<string, N>& data) {
    ofstream file;
    file.open("data/Category.json");
    file << to_json_str(data);
    file.close();
}

#pragma endregion Save Data as JSON (C++ Struct -> JSON)

/**
 * @brief Read a JSON file and return its contents as a string
 * @param path File path to the JSON file
 * @return std::string Content of the JSON file
 * @details Reads the file line by line and concatenates all lines into a single string
 */
string json_to_str(string path) {
    ifstream file;
    file.open(path);
    string data;
    string line;
    while (getline(file, line)) {
        data += line;
    }
    return data;
}

/**
 * @brief Initialize the database structure and create empty JSON files if they don't exist
 * @details 
 * - Creates a 'data' directory if it doesn't exist
 * - Creates empty JSON files for Month, Expense, and Category data
 * - Each file is initialized with an empty array "[]"
 * 
 * File structure created:
 * - data/
 *   - Month.json
 *   - Expense.json
 *   - Category.json
 */
void initialize_database() {
    // Create data directory if it doesn't exist
    if (!filesystem::exists("data")) {
        filesystem::create_directory("data");
    }

    // Initialize Budget.json with empty array if it doesn't exist
    if (!filesystem::exists("data/Budget.json")) {
        ofstream file;
        file.open("data/Budget.json");
        file << "[]";
        file.close();
    }

    // Initialize Expense.json with empty array if it doesn't exist
    if (!filesystem::exists("data/Expense.json")) {
        ofstream file;
        file.open("data/Expense.json");
        file << "[]";
        file.close();
    }

    // Initialize Category.json with empty array if it doesn't exist
    if (!filesystem::exists("data/Category.json")) {
        ofstream file;
        file.open("data/Category.json");
        file << "[]";
        file.close();
    }
}

void showLine() {
    cout << "----------------------------------------" << endl;
}

tm* get_current_date() {
    time_t t = time(0);
    return localtime(&t);
}

void showDate() {
    tm* now = get_current_date();
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
    cout << "4. Edit Budget" << endl;
    cout << "5. Exit" << endl;
    showLine();
    cout << "Enter your choice: ";
}

void waitEnter() {
    cout << endl << "Press Enter to continue...";
    cin.ignore();
    cin.get();
}

/**
 * @brief Create a new monthly budget by getting user input
 * @return Month struct containing the budget information
 * 
 * @details Prompts user for:
 * - Month (validates for range 1-12)
 * - Year
 * - Budget amount
 * 
 * @note Input validation is performed for month to ensure it's within valid range
 */
Budget create_budget() {
    int month, year;
    double amount;
    cout << "Enter month (1-12): ";
    cin >> month;
    while (month < 1 || month > 12) {
        cout << "Invalid month! Enter again (1-12): ";
        cin >> month;
    }
    cout << "Enter year: ";
    cin >> year;
    cout << "Enter budget: ";
    cin >> amount;
    return {amount, year, month};
}

void setBudget(int month, int year) {
    system("cls");

    // check JSON file exists or not
    if (filesystem::exists("data/Budget.json")) {

        // parse JSON file to vector of Month
        vector<Budget> budgets = parse_json<Budget>(json_to_str("data/Budget.json"));

        // if vector is empty but file exists, goto START
        if (budgets.empty()) {
            goto START;
        }

        for (Budget& x : budgets) {

            // if current month and year already set budget
            if (x.month == month && x.year == year) {
                int res;
                cout << "Budget for this month is already set to " << currentCurrency << fixed << setprecision(2) << " " << x.amount << endl;
                showLine();
                cout << "1. Change budget for this month\n";
                cout << "2. Set budget for other month\n";
                showLine();
                cout << "Enter your choice: ";
                cin >> res;
                system("cls");
                
                if (res == 1) {
                    cout << "Enter new budget for this month: ";
                    cin >> x.amount;
                    save_as_json(budgets);
                    break;
                }
                else if (res == 2){
                    Budget new_budget = create_budget();
                    if (new_budget.month == x.month && new_budget.year == x.year) {
                        setBudget(month, year);
                        return;
                    }
                    budgets.push_back(new_budget);
                    save_as_json(budgets);
                    break;
                }
            }
            else {
                budgets.push_back(create_budget());
                save_as_json(budgets);
                break;
            }
        }
    }
    else {
        START:
            vector<Budget> budgets = {create_budget()};
            save_as_json(budgets);
    }
}

void editBudget() {
    system("cls");
    cout << endl << "Edit Budget" << endl;
    showLine();
    
    // prompt user key the month and year
    int month, year;
    cout << "Enter month (1-12) to edit budget: ";
    cin >> month;
    while (month < 1 || month > 12) {
        cout << "Invalid month! Enter again (1-12): ";
        cin >> month;
    }
    
    cout << "Enter year: ";
    cin >> year;
    while (year < 2000) {
        cout << "Invalid year! Enter again (>= 2000): ";
        cin >> year;
    }
    
    // Check if Months.json exists
    if (filesystem::exists("data/Budget.json")) {
        vector<Budget> months = parse_json<Budget>(json_to_str("data/Budget.json"));
        
        //loop through to find the matching month and year
        bool found = false;
        for (Budget& m : months) { 
            if (m.month == month && m.year == year) {
                //let user edit it if budget found
                found = true;
                cout << "Current budget for " << month << "/" << year 
                     << " is: " << currentCurrency << fixed << setprecision(2) << m.amount << endl;
                
                double newBudget;
                cout << "Enter new budget amount: " << currentCurrency << " ";
                cin >> newBudget;
                while (newBudget < 0) { //validate the input
                    cout << "Invalid amount! Enter again: " << currentCurrency << " ";
                    cin >> newBudget;
                }

                // save the budget in JSON file
                m.amount = newBudget;
                save_as_json(months);
                cout << "Budget updated!!!" << endl;
                waitEnter();
                break;
            }
        }

        // let user set budget for future if not found
        if (!found) {
            cout << "No budget found for " << month << "/" << year << "." << endl;
            char addNewBudget;
            cout << "Would you like to set a new budget for this month? (y/n): ";
            cin >> addNewBudget;

            if (addNewBudget == 'y' || addNewBudget == 'Y') {
                double newBudget;
                cout << "Enter new budget amount: " << currentCurrency << " ";
                cin >> newBudget;
                while (newBudget < 0) {
                    cout << "Invalid amount! Enter again: " << currentCurrency << " ";
                    cin >> newBudget;
                }
                // save to json file
                months.push_back({newBudget, year, month});
                save_as_json(months);
                cout << "Budget set successfully!" << endl;
                waitEnter();
            }
        }
    } else {
        cout << "No budget data found!" << endl;
        waitEnter();
    }
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
            save_as_json(cats); 
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
                    save_as_json(cats); 
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

    // read from the json file 
    vector<Expense> expenseRecords;
    if (filesystem::exists("data/Expense.json")) {
        expenseRecords = parse_json<Expense>(json_to_str("data/Expense.json"));
    }

    // calc the newID for the record
    int newId = 1;
    if (!expenseRecords.empty()) {
        newId = expenseRecords.back().id + 1; //access the id from the last records from the vector and +1
    }

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

        tm* now = get_current_date();
        //get current month's budget
        double currentBudget = 0;
        if (filesystem::exists("data/Budget.json")) {
            vector<Budget> budgets = parse_json<Budget>(json_to_str("data/Budget.json"));
            for (const auto& b : budgets) {
                if (b.month == (now->tm_mon + 1) && b.year == (now->tm_year + 1900)) {
                    currentBudget = b.amount;
                    break;
                }
            }
        }

        // Budget warning check
        if (currentBudget > 0 && (totalExpense * curr.rate) > currentBudget) {
            cout << "\nWARNING: This expense will exceed your budget of "
                 << curr.symbol << fixed << setprecision(2) << currentBudget << "!\n";
            cout << "Do you want to continue? (y/n): ";
            char confirm;
            cin >> confirm;
            if (confirm != 'y' && confirm != 'Y') {
                continue;
            }
        }

        expenses[catNum - 1].push_back(amount / curr.rate);
        
        
        // Get remarks for the expense
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        string remarks;
        cout << "Enter remarks: ";
        getline(cin, remarks);

        // create the new expense record to be added
        Expense newExpense{
            newId++,
            cats[catNum - 1], //category
            amount,
            remarks,
            now->tm_mday,
            now->tm_mon + 1,
            now->tm_year + 1900
        };

        //add to expense records and save to json file
        expenseRecords.push_back(newExpense);
        save_as_json(expenseRecords);

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
    
    tm* now = get_current_date();
    double currentBudget = 0;
    if (filesystem::exists("data/Budget.json")) {
            vector<Budget> budgets = parse_json<Budget>(json_to_str("data/Budget.json"));
            for (const auto& b : budgets) {
                if (b.month == (now->tm_mon + 1) && b.year == (now->tm_year + 1900)) {
                    currentBudget = b.amount;
                    break;
                }
            }
        }

    if (currentBudget > 0) {
        cout << "Budget: " << curr.symbol << setprecision(2) << currentBudget << endl;
        double remaining = currentBudget - (total * curr.rate);
        cout << "Remaining: " << curr.symbol << setprecision(2) << remaining << endl;
    }
}

/**
 * @brief Initialize expense categories, either from existing file or default values
 * @return vector<string> Vector containing expense categories
 * 
 * @details Function follows this logic:
 * 1. If Category.json exists:
 *    - Reads and parses existing categories
 *    - If file is empty, loads default categories
 * 2. If Category.json doesn't exist:
 *    - Creates new file with default categories
 * 
 * @note Default categories are loaded from default_categories array
 * @see default_categories
 * @see save_as_json
 * @see parse_json
 */
vector<string> category_init() {
    vector<string> categories;
    // Check if category file exists
    if (filesystem::exists("data/Category.json")) {
        // Read and parse existing categories
        categories = parse_json<string>(json_to_str("data/Category.json"));
        // If file is empty, use default categories
        if (categories.empty()) {
            categories = vector<string>(default_categories.begin(), default_categories.end());
            save_as_json(default_categories);
        }
    } 
     // If file doesn't exist, create new with default categories
     else {
        categories = vector<string>(default_categories.begin(), default_categories.end());
        save_as_json(default_categories);
    }

    return categories;
}

int main() {
    initialize_database();
    
    tm* current_date = get_current_date();
    const int month = current_date->tm_mon + 1;
    const int year = current_date->tm_year + 1900;

    vector<string> categories = category_init();

    vector<vector<double>> expenses(categories.size());
    int choice;
    
    do {
        showMenu();
        cin >> choice;
        
        if (choice == 1) {
            setBudget(month, year);
        }
        else if (choice == 2) {
            addExpense(expenses, categories);
        }
        else if (choice == 3) {
            appSettings(categories, expenses);
        }
        else if (choice == 4){
            editBudget();
        }
        else if (choice == 5) {
            showSummary(categories, expenses);
            cout << endl << "Goodbye!" << endl;
        }
        
    } while (choice != 5);
    
    return 0;
}