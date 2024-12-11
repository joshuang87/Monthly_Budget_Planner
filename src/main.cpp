#include <iostream>
#include <ctime>
using namespace std;

void CurrentDate();

int main() {

    int choice;

    do {

        CurrentDate();

        cout << "Monthly Budget Planner\n" 
             << "1. Set Budget\n"
             << "2. Make Expenses Record\n"
             << "3. App Setting\n"
             << "4. Exit\n"
             << "Enter your choice: ";
        cin  >> choice;

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