// main.cpp
// Grab Fare Calculator - menu and user interaction.
// LDCW6123 Group Project, Part 2.

#include <cctype>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "fare_calculator.h"

using namespace std;

// One entry in this session's trip history
struct TripRecord {
    string description;     // e.g. "GrabCar, 12.50 km, 25 min"
    double fare;            // the fare shown to the user
    double savingVsTaxi;    // comparisons only: budget taxi fare minus GrabCar fare
    bool isComparison;
};

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
    if (getTimeBand(trip.pickupHour) == PEAK) {
        cout << "     Traffic is usually heavy now, so allow more minutes: GrabCar charges per minute.\n";
    }
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
void estimateGrabFare(vector<TripRecord>& history) {
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

    TripRecord record;
    record.description = rideTypeName(type) + ", " + formatNumber(trip.distanceKm) + " km, "
                         + to_string(trip.durationMin) + " min";
    record.fare = fare.total;
    record.savingVsTaxi = 0.0;
    record.isComparison = false;
    history.push_back(record);
}

// ---------------------------------------------------------------
// Menu option 2: compare Grab with a traditional taxi
// ---------------------------------------------------------------
void compareGrabWithTaxi(vector<TripRecord>& history) {
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

    TripRecord record;
    record.description = "Comparison, " + formatNumber(trip.distanceKm) + " km, "
                         + to_string(trip.durationMin) + " min (cheapest: "
                         + rideTypeName(fares[cheapest].rideType) + ")";
    record.fare = fares[cheapest].total;
    record.savingVsTaxi = difference;
    record.isComparison = true;
    history.push_back(record);
}

// ---------------------------------------------------------------
// Menu option 5 and exit: trip history for this session
// ---------------------------------------------------------------
void showTripHistory(const vector<TripRecord>& history) {
    cout << "\n--- TRIP HISTORY (this session) ---\n";
    if (history.empty()) {
        cout << "  No trips yet. Try option 1 or 2 first.\n";
        return;
    }
    // Wide enough for the longest description, e.g.
    // "Comparison, 300.00 km, 600 min (cheapest: Teksi 1Malaysia)"
    const int DESCRIPTION_WIDTH = 60;
    for (size_t i = 0; i < history.size(); i++) {
        cout << "  " << (i + 1) << ". " << left << setw(DESCRIPTION_WIDTH) << history[i].description
             << " " << right << "RM" << setw(8) << formatNumber(history[i].fare) << "\n";
    }
}

void showSessionSummary(const vector<TripRecord>& history) {
    if (history.empty()) {
        return;
    }
    int comparisons = 0;
    double totalSaving = 0.0;
    for (const TripRecord& record : history) {
        if (record.isComparison) {
            comparisons++;
            totalSaving += record.savingVsTaxi;
        }
    }

    cout << "\n--- SESSION SUMMARY ---\n";
    cout << "  Trips estimated: " << history.size() << " (" << comparisons << " comparison";
    if (comparisons != 1) {
        cout << "s";
    }
    cout << ")\n";
    if (comparisons > 0) {
        if (totalSaving >= 0) {
            cout << "  Choosing GrabCar instead of a budget taxi would save you RM"
                 << formatNumber(totalSaving) << " in total.\n";
        } else {
            cout << "  A budget taxi would have cost RM" << formatNumber(-totalSaving)
                 << " less in total for these trips.\n";
        }
    }
}

// ---------------------------------------------------------------
// Menu option 3: rate card
// ---------------------------------------------------------------
void showRateCard() {
    cout << "\n--- RATE CARD (Klang Valley) ---\n\n";
    cout << "  GRAB - upfront price     Base     Per km   Per min  Minimum\n";
    cout << "  GrabCar                  " << setw(4) << formatNumber(GRABCAR_BASE) << "     "
         << formatNumber(GRABCAR_PER_KM) << "     " << formatNumber(GRABCAR_PER_MIN) << "     "
         << formatNumber(GRABCAR_MINIMUM) << "\n";
    cout << "  GrabCar Premium          " << setw(4) << formatNumber(PREMIUM_BASE) << "     "
         << formatNumber(PREMIUM_PER_KM) << "     " << formatNumber(PREMIUM_PER_MIN) << "    "
         << formatNumber(PREMIUM_MINIMUM) << "\n";
    cout << "  Demand surge: normal x" << formatNumber(SURGE_NORMAL, 1)
         << ", high x" << formatNumber(SURGE_HIGH, 1)
         << ", very high x" << formatNumber(SURGE_VERY_HIGH, 1) << "\n";
    cout << "  GrabCar rates apply all day since January 2023. Grab's peak hours\n";
    cout << "  (7-9am, 5-8pm) cost more because trips take longer in traffic.\n\n";

    cout << "  TAXI - meter             Flag fall  Per km   Per min (waiting)\n";
    cout << "  Budget taxi              " << formatNumber(BUDGET_TAXI_FLAG_FALL) << "       "
         << formatNumber(BUDGET_TAXI_PER_KM) << "     " << formatNumber(BUDGET_TAXI_PER_MIN) << "\n";
    cout << "  Teksi 1Malaysia          " << formatNumber(TEKSI1M_FLAG_FALL) << "       "
         << formatNumber(TEKSI1M_PER_KM) << "     " << formatNumber(TEKSI1M_PER_MIN) << "\n";
    cout << "  Flag fall includes the first 1 km. Midnight surcharge +50% (12am-6am).\n";
    cout << "  Phone booking fee RM" << formatNumber(PHONE_BOOKING_FEE) << ".\n\n";

    cout << "  Sources: The Star (2023) for GrabCar; TaxiFareFinder for GrabCar Premium;\n";
    cout << "  SPAD rates reported by The Star and The Edge (2015) for taxis.\n";
    cout << "  Surge levels are simplified for this program.\n";
}

// ---------------------------------------------------------------
// Menu option 4: link back to Part 1 (the innovation poster)
// ---------------------------------------------------------------
void showDisruptionSummary() {
    cout << "\n--- WHY DID GRAB DISRUPT TAXIS? ---\n";
    cout << "Christensen's disruptive innovation model: a small newcomer first serves\n";
    cout << "customers the big players overlook, then keeps improving until mainstream\n";
    cout << "customers switch to it.\n\n";

    cout << "  2012  MyTeksi launches in Kuala Lumpur - book a taxi safely from your phone\n";
    cout << "  2014  GrabCar launches as a premium service using rental-company cars\n";
    cout << "  2015  Private car owners start driving for GrabCar, at lower prices\n";
    cout << "  2016  Rebrands as Grab and adds cashless payment\n";
    cout << "  2017  Malaysia legalises e-hailing (27 July 2017)\n";
    cout << "  2018  Grab takes over Uber's Southeast Asia business\n";
    cout << "  2019  E-hailing drivers now need a PSV licence and an EVP permit\n";
    cout << "  2021  Grab lists on Nasdaq (ticker: GRAB)\n";
    cout << "  2023  Grab-led GXBank becomes Malaysia's first digital bank to launch\n\n";

    cout << "What passengers got that most taxis did not offer:\n";
    cout << "  - an upfront price before the trip (no meter surprises)\n";
    cout << "  - the driver's name, car plate and live GPS tracking\n";
    cout << "  - rides in places and at times when taxis were hard to find\n";
    cout << "  - cashless payment\n\n";

    cout << "Impact: taxis on Malaysian roads fell from about 120,000 to about 40,000\n";
    cout << "(Gabungan Teksi Malaysia, reported in 2024).\n";
    cout << "Try option 2 to see the price difference for yourself.\n";
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

    vector<TripRecord> history;
    int choice = -1;
    while (choice != 0) {
        showMenu();
        choice = readInt("Enter your choice (0-5): ", 0, 5);

        switch (choice) {
            case 1:
                estimateGrabFare(history);
                break;
            case 2:
                compareGrabWithTaxi(history);
                break;
            case 3:
                showRateCard();
                break;
            case 4:
                showDisruptionSummary();
                break;
            case 5:
                showTripHistory(history);
                break;
            case 0:
                showSessionSummary(history);
                cout << "\nThank you for using the Grab Fare Calculator.\n";
                break;
            default:
                cout << "Invalid choice. Please enter a number from 0 to 5.\n";
        }
    }
    return 0;
}
