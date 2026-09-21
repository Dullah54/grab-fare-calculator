// main.cpp
// Grab Fare Calculator - menu and user interaction.
// LDCW6123 Group Project, Part 2.

#include <cctype>
#include <cstdlib>
#include <iostream>
#include <string>

#include "fare_calculator.h"

using namespace std;

// ---------------------------------------------------------------
// Input helpers
// Every question reads a whole line, checks it, and asks again if
// the answer is not valid, so wrong input never crashes the program.
// ---------------------------------------------------------------

// Reads one full line of text from the user
string readLine(const string& prompt) {
    cout << prompt;
    string line;
    if (!getline(cin, line)) {
        // Input stream closed (e.g. end of a test file): exit safely
        cout << "\nNo more input. Goodbye!\n";
        exit(0);
    }
    return line;
}

// Removes spaces at the start and end of a string
string trim(const string& text) {
    size_t start = text.find_first_not_of(" \t\r");
    if (start == string::npos) {
        return "";
    }
    size_t end = text.find_last_not_of(" \t\r");
    return text.substr(start, end - start + 1);
}

// Asks for a whole number between minValue and maxValue
int readInt(const string& prompt, int minValue, int maxValue) {
    while (true) {
        string answer = trim(readLine(prompt));
        try {
            size_t charsUsed = 0;
            int value = stoi(answer, &charsUsed);
            if (charsUsed == answer.size() && value >= minValue && value <= maxValue) {
                return value;
            }
        } catch (...) {
            // not a number - fall through to the error message
        }
        cout << "  ! Please enter a whole number from " << minValue << " to " << maxValue << ".\n";
    }
}

// Asks for a decimal number between minValue and maxValue
double readDouble(const string& prompt, double minValue, double maxValue) {
    while (true) {
        string answer = trim(readLine(prompt));
        try {
            size_t charsUsed = 0;
            double value = stod(answer, &charsUsed);
            if (charsUsed == answer.size() && value >= minValue && value <= maxValue) {
                return value;
            }
        } catch (...) {
            // not a number - fall through to the error message
        }
        cout << "  ! Please enter a number from " << minValue << " to " << maxValue << ".\n";
    }
}

// Asks a yes/no question and accepts y, yes, n or no in any case
bool readYesNo(const string& prompt) {
    while (true) {
        string answer = trim(readLine(prompt));
        for (char& c : answer) {
            c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
        }
        if (answer == "y" || answer == "yes") {
            return true;
        } else if (answer == "n" || answer == "no") {
            return false;
        }
        cout << "  ! Please answer y or n.\n";
    }
}

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
        choice = readInt("Enter your choice (0-5): ", 0, 5);

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
