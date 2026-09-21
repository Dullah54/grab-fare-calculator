// main.cpp
// Grab Fare Calculator - menu and user interaction.
// LDCW6123 Group Project, Part 2.

#include <cctype>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>
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

// ---------------------------------------------------------------
// Output helpers
// ---------------------------------------------------------------

void printDivider() {
    cout << "  ------------------------------------------------------\n";
}

// Formats a number with a fixed number of decimal places.
// Uses its own stream so cout's number format is never changed.
string formatNumber(double value, int decimals = 2) {
    ostringstream text;
    text << fixed << setprecision(decimals) << value;
    return text.str();
}

// Prints one line of a receipt, e.g. "Base fare ........ RM   2.00"
void printMoneyLine(const string& label, double amount) {
    cout << "  " << left << setw(42) << label << right;
    if (amount < 0) {
        cout << "-RM" << setw(8) << formatNumber(-amount) << "\n";
    } else {
        cout << " RM" << setw(8) << formatNumber(amount) << "\n";
    }
}

// Prints the full breakdown of one fare
void printFareBreakdown(const FareBreakdown& fare) {
    cout << "\n";
    printDivider();
    cout << "  FARE BREAKDOWN - " << rideTypeName(fare.rideType) << "\n";
    printDivider();
    printMoneyLine("Base fare", fare.baseFare);
    printMoneyLine("Distance  " + formatNumber(fare.chargedKm) + " km x RM" + formatNumber(fare.ratePerKm),
                   fare.distanceCharge);
    printMoneyLine("Time      " + formatNumber(fare.chargedMinutes, 1) + " min x RM" + formatNumber(fare.ratePerMin),
                   fare.timeCharge);
    if (fare.surgeOrSurcharge > 0) {
        printMoneyLine("Demand surge", fare.surgeOrSurcharge);
    }
    if (fare.minimumFareApplied) {
        cout << "  (Minimum fare applied)\n";
    }
    printDivider();
    printMoneyLine("TOTAL", fare.total);
    printDivider();
}

// ---------------------------------------------------------------
// Menu option 1: estimate a Grab fare
// ---------------------------------------------------------------
void estimateGrabFare() {
    cout << "\n--- ESTIMATE A GRAB RIDE FARE ---\n";
    Trip trip;
    trip.distanceKm  = readDouble("Trip distance in km (0.5 - 300): ", 0.5, 300.0);
    trip.durationMin = readInt("Estimated trip time in minutes (1 - 600): ", 1, 600);
    trip.pickupHour  = readInt("Pickup hour in 24-hour time (0 - 23, e.g. 8 = 8am, 18 = 6pm): ", 0, 23);
    cout << "  -> Time band: " << timeBandName(getTimeBand(trip.pickupHour)) << "\n";

    cout << "How busy is it right now? (demand affects Grab's price)\n";
    cout << "  1. Normal\n";
    cout << "  2. High       (e.g. rush hour, events)  x" << formatNumber(SURGE_HIGH, 1) << "\n";
    cout << "  3. Very high  (e.g. heavy rain)         x" << formatNumber(SURGE_VERY_HIGH, 1) << "\n";
    trip.demand = static_cast<DemandLevel>(readInt("Choose demand level (1-3): ", 1, 3));

    cout << "Ride type:\n";
    cout << "  1. GrabCar          (standard 4-seater)\n";
    cout << "  2. GrabCar Premium  (newer, higher-end car)\n";
    int typeChoice = readInt("Choose ride type (1-2): ", 1, 2);
    RideType type = (typeChoice == 1) ? GRABCAR : GRABCAR_PREMIUM;

    FareBreakdown fare = calculateGrabFare(trip, type);
    printFareBreakdown(fare);
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
                estimateGrabFare();
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
