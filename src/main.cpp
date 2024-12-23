#include <iostream>
#include <vector>
#include <map>
#include <iomanip>
#include <ctime>
#include <string>
#include <limits>
#include <filesystem>
#include <fstream>
#include <type_traits>
#include <sstream>
using namespace std;
// Forward declarations for JSON file operations
std::string read_json_file(const std::string& path);
template<typename T>
void save_json_file(const std::string& path, const std::vector<T>& items);
template<typename T>
std::vector<T> parse_json_array(const std::string& data);

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

template<typename T>// Generic JSON parser interface
struct JsonParser {
    static T parse(const std::string& json_str);
    static std::string to_json(const T& obj);
};

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
 * @brief Template specialization for parsing Month JSON data
 * @param json_str JSON string containing month data
 * @return vector<Month> Vector of Month objects
 * @details Expects JSON format:
 *          {"month": int, "year": int, "budget": double}
 */
template <>
vector<Month> parse_json<Month>(const string& json_str) {
    vector<Month> months;
    stringstream ss(json_str);
    string line;

    while (getline(ss, line, '{')) {
        // Skip non-month entries
        if (line.find("month") == string::npos) continue;

        Month m;
        size_t pos;

        // Parse month value
        pos = line.find("month");
        m.value = stoi(line.substr(pos + 7, line.find(",", pos) - pos - 7));

        // Parse month year
        pos = line.find("year");
        m.year = stoi(line.substr(pos + 6, line.find(",", pos) - pos - 6));

        // Parse month budget
        pos = line.find("budget");
        m.budget = stod(line.substr(pos + 8, line.find("}", pos) - pos - 8));

        months.push_back(m);
    }

    return months;
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
 * @brief Template specialization for saving Month data to JSON file
 * @param data Vector of Month objects to be saved
 * @details Saves to "data/Month.json" using the Month to JSON string converter
 * @see to_json_str<Month>
 */
template <>
void save_as_json<Month>(const vector<Month>& data) {
    ofstream file;
    file.open("data/Month.json");
    file << to_json_str<Month>(data);
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

    // Initialize Month.json with empty array if it doesn't exist
    if (!filesystem::exists("data/Month.json")) {
        ofstream file;
        file.open("data/Month.json");
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
    cout << "4. Exit" << endl;
    showLine();
    cout << "Enter your choice: ";
}

void waitEnter() {
    cout << endl << "Press Enter to continue...";
    cin.ignore();
    cin.get();
}

void setBudget(int month, int year) {
    system("cls");
    cout << endl << "Set Budget" << endl;
    showLine();
    
    // enter month and year
    int input_month, input_year;
    cout << "Enter month (1-12): ";
    cin >> input_month;
    while (input_month < 1 || input_month > 12) {
        cout << "Invalid month! Enter again (1-12): ";
        cin >> input_month;
    }// Check if budget exists
    
    cout << "Enter year: ";
    cin >> input_year;
    while (input_year < 2000) {
        cout << "Invalid year! Enter again (>= 2000): ";
        cin >> input_year;
    }
    
    if (filesystem::exists("data/Months.json")) {
        string json_data = read_json_file("data/Months.json");
        vector<Month> months = parse_json_array<Month>(json_data);
        
        // check if recorded
        for (Month x : months) {
            if (x.value == input_month && x.year == input_year) {
                cout << "Budget for " << input_month << "/" << input_year 
                     << " is already set to " << currentCurrency 
                     << fixed << setprecision(2) << x.budget << endl;
                budget = x.budget;  // 设置全局budget
                waitEnter();
                return;
            }
        }
        
        // if not yet add budget
        double new_budget;
        cout << "Enter budget amount: " << currencies[currentCurrency].symbol << " ";
        cin >> new_budget;
        while (new_budget < 0) {
            cout << "Invalid amount! Enter again: " << currencies[currentCurrency].symbol << " ";
            cin >> new_budget;
        }
        
        budget = new_budget;
        months.push_back({input_month, input_year, new_budget});
        save_json_file("data/Months.json", months);
        cout << "Budget set successfully!" << endl;
        waitEnter();
        
    } else {
        // create new budget amount
        double new_budget;
        cout << "Enter budget amount: " << currencies[currentCurrency].symbol << " ";
        cin >> new_budget;
        while (new_budget < 0) {
            cout << "Invalid amount! Enter again: " << currencies[currentCurrency].symbol << " ";
            cin >> new_budget;
        }
        
        budget = new_budget;
        vector<Month> months = {{input_month, input_year, new_budget}};
        save_json_file("data/Months.json", months);
        cout << "Budget set successfully!" << endl;
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
    string expense_file = "data/Expenses.json";
    vector<Expense> all_expenses;
    
    // Load existing expenses if file exists
    if (filesystem::exists(expense_file)) {
        string json_data = read_json_file(expense_file);
        all_expenses = parse_json_array<Expense>(json_data);
    }

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

        // date on time
        time_t now = time(0);
        tm* ltm = localtime(&now);
        string date = to_string(1900 + ltm->tm_year) + "-" + 
                     to_string(1 + ltm->tm_mon) + "-" + 
                     to_string(ltm->tm_mday);

        // create new expense
        Expense new_expense{
            catNum - 1,  // category_id
            amount / curr.rate,
            date
        };
        
        // save the json file
        all_expenses.push_back(new_expense);
        expenses[catNum - 1].push_back(amount / curr.rate);
        save_json_file(expense_file, all_expenses);
        
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
    
    
    if (filesystem::exists("data/Months.json")) {
        string json_data = read_json_file("data/Months.json");
        vector<Month> months = parse_json_array<Month>(json_data);
        for (const Month& m : months) {
            if (m.value == month && m.year == year) {
                budget = m.budget;
                break;
            }
        }
    }
    
    if (filesystem::exists("data/Expenses.json")) {
        string json_data = read_json_file("data/Expenses.json");
        vector<Expense> all_expenses = parse_json_array<Expense>(json_data);
    
        for (const auto& expense : all_expenses) {
            if (expense.category_id >= 0 && expense.category_id < expenses.size()) {
                expenses[expense.category_id].push_back(expense.amount);
            }
        }
    }
    
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
        else if (choice == 4) {
            showSummary(categories, expenses);
            cout << endl << "Goodbye!" << endl;
        }
        
    } while (choice != 4);
    
    return 0;
}
