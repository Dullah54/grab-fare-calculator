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
    if (isTaxi(fare.rideType)) {
        printMoneyLine("Flag fall (includes first 1 km)", fare.baseFare);
        printMoneyLine("Distance  " + formatNumber(fare.chargedKm) + " km x RM" + formatNumber(fare.ratePerKm),
                       fare.distanceCharge);
        printMoneyLine("Waiting   " + formatNumber(fare.chargedMinutes, 1) + " min x RM" + formatNumber(fare.ratePerMin),
                       fare.timeCharge);
    } else {
        printMoneyLine("Base fare", fare.baseFare);
        printMoneyLine("Distance  " + formatNumber(fare.chargedKm) + " km x RM" + formatNumber(fare.ratePerKm),
                       fare.distanceCharge);
        printMoneyLine("Time      " + formatNumber(fare.chargedMinutes, 1) + " min x RM" + formatNumber(fare.ratePerMin),
                       fare.timeCharge);
    }
    if (fare.surgeOrSurcharge > 0) {
        if (isTaxi(fare.rideType)) {
            printMoneyLine("Midnight surcharge (+50%)", fare.surgeOrSurcharge);
        } else {
            printMoneyLine("Demand surge", fare.surgeOrSurcharge);
        }
    }
    if (fare.bookingFee > 0) {
        printMoneyLine("Phone booking fee", fare.bookingFee);
    }
    if (fare.minimumFareApplied) {
        cout << "  (Minimum fare applied)\n";
    }
    if (fare.discount > 0) {
        printMoneyLine("Promo discount", -fare.discount);
    }
    if (fare.toll > 0) {
        printMoneyLine("Toll", fare.toll);
    }
    if (fare.roundingAdjustment != 0) {
        printMoneyLine("Cash rounding (nearest 5 sen)", fare.roundingAdjustment);
    }
    printDivider();
    printMoneyLine("TOTAL", fare.total);
    printDivider();
}

// ---------------------------------------------------------------
// Trip questions shared by menu options 1 and 2
// ---------------------------------------------------------------

// Distance, time and pickup hour
void readTripBasics(Trip& trip) {
    trip.distanceKm  = readDouble("Trip distance in km (0.5 - 300): ", 0.5, 300.0);
    trip.durationMin = readInt("Estimated trip time in minutes (1 - 600): ", 1, 600);
    trip.pickupHour  = readInt("Pickup hour in 24-hour time (0 - 23, e.g. 8 = 8am, 18 = 6pm): ", 0, 23);
    cout << "  -> Time band: " << timeBandName(getTimeBand(trip.pickupHour)) << "\n";
    trip.tollRM      = readDouble("Toll charges on the route in RM (0 if none): ", 0.0, 100.0);
}

void readDemandLevel(Trip& trip) {
    cout << "How busy is it right now? (demand affects Grab's price)\n";
    cout << "  1. Normal\n";
    cout << "  2. High       (e.g. rush hour, events)  x" << formatNumber(SURGE_HIGH, 1) << "\n";
    cout << "  3. Very high  (e.g. heavy rain)         x" << formatNumber(SURGE_VERY_HIGH, 1) << "\n";
    trip.demand = static_cast<DemandLevel>(readInt("Choose demand level (1-3): ", 1, 3));
}

void readTrafficLevel(Trip& trip) {
    cout << "How is the traffic? (a taxi meter charges waiting time in slow traffic)\n";
    cout << "  1. Light\n";
    cout << "  2. Moderate\n";
    cout << "  3. Heavy\n";
    trip.traffic = static_cast<TrafficLevel>(readInt("Choose traffic level (1-3): ", 1, 3));
}

// ---------------------------------------------------------------
// Menu option 1: estimate a Grab fare
// ---------------------------------------------------------------
void estimateGrabFare() {
    cout << "\n--- ESTIMATE A GRAB RIDE FARE ---\n";
    Trip trip;
    readTripBasics(trip);
    readDemandLevel(trip);

    cout << "Ride type:\n";
    cout << "  1. GrabCar          (standard 4-seater)\n";
    cout << "  2. GrabCar Premium  (newer, higher-end car)\n";
    int typeChoice = readInt("Choose ride type (1-2): ", 1, 2);
    RideType type = (typeChoice == 1) ? GRABCAR : GRABCAR_PREMIUM;

    // Promo code: keep asking until it is valid or left empty
    while (true) {
        trip.promoCode = trim(readLine("Promo code (try STUDENT10 or NEWRIDER, press Enter to skip): "));
        if (trip.promoCode.empty() || isValidPromoCode(trip.promoCode)) {
            break;
        }
        cout << "  ! Sorry, \"" << trip.promoCode << "\" is not a valid promo code.\n";
    }

    cout << "Payment method:\n";
    cout << "  1. GrabPay / card  (cashless)\n";
    cout << "  2. Cash            (rounded to the nearest 5 sen)\n";
    trip.payment = static_cast<PaymentMethod>(readInt("Choose payment method (1-2): ", 1, 2));

    FareBreakdown fare = calculateGrabFare(trip, type);
    printFareBreakdown(fare);
    if (!trip.promoCode.empty()) {
        cout << "  Promo code " << toUpperCase(trip.promoCode) << " applied.\n";
    }
    cout << "  This is an upfront price: it will not change because of traffic.\n";
}

// ---------------------------------------------------------------
// Menu option 2: compare Grab with a traditional taxi
// ---------------------------------------------------------------
void compareGrabWithTaxi() {
    cout << "\n--- COMPARE GRAB WITH A TRADITIONAL TAXI ---\n";
    Trip trip;
    readTripBasics(trip);
    readTrafficLevel(trip);
    readDemandLevel(trip);
    trip.bookedByPhone = readYesNo("Would you book the taxi by phone? (y/n): ");

    const int OPTION_COUNT = 4;
    FareBreakdown fares[OPTION_COUNT] = {
        calculateGrabFare(trip, GRABCAR),
        calculateGrabFare(trip, GRABCAR_PREMIUM),
        calculateTaxiFare(trip, BUDGET_TAXI),
        calculateTaxiFare(trip, TEKSI_1MALAYSIA)
    };

    // Find the cheapest option
    int cheapest = 0;
    for (int i = 1; i < OPTION_COUNT; i++) {
        if (fares[i].total < fares[cheapest].total) {
            cheapest = i;
        }
    }

    cout << "\n";
    printDivider();
    cout << "  " << left << setw(20) << "Ride option" << setw(14) << "Estimated fare" << "  Pricing\n";
    printDivider();
    for (int i = 0; i < OPTION_COUNT; i++) {
        string pricing = isTaxi(fares[i].rideType) ? "meter (known at the end)" : "upfront (known before booking)";
        cout << "  " << left << setw(20) << rideTypeName(fares[i].rideType)
             << "RM" << right << setw(8) << formatNumber(fares[i].total) << "    " << pricing;
        if (i == cheapest) {
            cout << "  <- cheapest";
        }
        cout << "\n";
    }
    printDivider();

    // Compare the everyday choices: GrabCar vs a budget taxi
    double grabFare = fares[0].total;
    double taxiFare = fares[2].total;
    double difference = taxiFare - grabFare;
    if (difference > 0) {
        cout << "  GrabCar saves you RM" << formatNumber(difference) << " ("
             << formatNumber(difference / taxiFare * 100, 0) << "%) compared with a budget taxi.\n";
    } else if (difference < 0) {
        cout << "  A budget taxi is RM" << formatNumber(-difference) << " cheaper this time";
        if (trip.demand != NORMAL_DEMAND) {
            cout << " because of Grab's surge pricing";
        }
        cout << ".\n";
    } else {
        cout << "  GrabCar and a budget taxi cost the same for this trip.\n";
    }

    if (readYesNo("\nShow the full breakdown for each option? (y/n): ")) {
        for (int i = 0; i < OPTION_COUNT; i++) {
            printFareBreakdown(fares[i]);
        }
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
                estimateGrabFare();
                break;
            case 2:
                compareGrabWithTaxi();
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
