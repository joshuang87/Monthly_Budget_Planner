#include <iostream>
#include <ctime>
#include <limits>
#include <vector>
using namespace std;

void CurrentDate();
void userinput_expenses(vector<vector<double>> &categoryexpenses, const vector<string> &category);

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
            cout<<"havent"<<endl;
            break;

            case 4:
            cout<<"Bye!"<<endl;
            break;
        }

    } while (choice != 4);


    cout << "\nFinal Expenses Record\n";
    for (int i = 0; i < category.size(); i++) {
        cout << "Category: " << category[i] << "\n";
        for (int j = 0; j < categoryexpenses[i].size(); j++) {
            cout << "  Expense #" << j + 1 << " : RM " << categoryexpenses[i][j] << endl;
        }
    }

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
