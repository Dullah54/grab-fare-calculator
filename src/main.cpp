// main.cpp
// Grab Fare Calculator - menu and user interaction.
// LDCW6123 Group Project, Part 2.

#include <iostream>
#include <string>

#include "fare_calculator.h"

using namespace std;

void showBanner() {
    cout << "==============================================================\n";
    cout << "          GRAB FARE CALCULATOR  -  Klang Valley\n";
    cout << "   Inspired by how Grab disrupted the traditional taxi industry\n";
    cout << "==============================================================\n";
}

void showMenu() {
    cout << "\nMAIN MENU\n";
    cout << "  1. Estimate a Grab ride fare\n";
    cout << "  2. Compare Grab with a traditional taxi\n";
    cout << "  3. View rate card\n";
    cout << "  4. Why did Grab disrupt taxis? (Part 1 summary)\n";
    cout << "  5. View trip history\n";
    cout << "  0. Exit\n";
}

int main() {
    showBanner();

    int choice = -1;
    while (choice != 0) {
        showMenu();
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                cout << "Grab fare estimate - coming soon.\n";
                break;
            case 2:
                cout << "Grab vs taxi comparison - coming soon.\n";
                break;
            case 3:
                cout << "Rate card - coming soon.\n";
                break;
            case 4:
                cout << "Part 1 summary - coming soon.\n";
                break;
            case 5:
                cout << "Trip history - coming soon.\n";
                break;
            case 0:
                cout << "Thank you for using the Grab Fare Calculator.\n";
                break;
            default:
                cout << "Invalid choice. Please enter a number from 0 to 5.\n";
        }
    }
    return 0;
}
