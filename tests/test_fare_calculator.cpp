// test_fare_calculator.cpp
// Automated tests for the fare calculation functions.
// Every expected value below was worked out by hand from the rate card.
//
// Build and run:
//   g++ -std=c++17 -Wall -Wextra -Isrc -o run_tests tests/test_fare_calculator.cpp src/fare_calculator.cpp
//   ./run_tests

#include <cmath>
#include <iomanip>
#include <iostream>
#include <string>

#include "fare_calculator.h"

using namespace std;

int testsRun = 0;
int testsPassed = 0;

// Checks that two ringgit amounts are equal (to the nearest sen)
void checkMoney(const string& name, double actual, double expected) {
    testsRun++;
    bool passed = fabs(actual - expected) < 0.001;
    if (passed) {
        testsPassed++;
    }
    cout << (passed ? "  [PASS] " : "  [FAIL] ") << left << setw(52) << name
         << "expected RM" << fixed << setprecision(2) << expected
         << ", got RM" << actual << "\n";
}

// Checks a true/false condition
void checkTrue(const string& name, bool condition) {
    testsRun++;
    if (condition) {
        testsPassed++;
    }
    cout << (condition ? "  [PASS] " : "  [FAIL] ") << name << "\n";
}

// Builds a trip with default values that each test can change
Trip makeTrip(double km, int minutes, int hour) {
    Trip trip;
    trip.distanceKm = km;
    trip.durationMin = minutes;
    trip.pickupHour = hour;
    return trip;
}

int main() {
    cout << "Running fare calculator tests...\n\n";

    cout << "Time bands\n";
    checkTrue("12am is midnight band", getTimeBand(0) == MIDNIGHT);
    checkTrue("5am is midnight band", getTimeBand(5) == MIDNIGHT);
    checkTrue("6am is off-peak", getTimeBand(6) == OFF_PEAK);
    checkTrue("8am is peak", getTimeBand(8) == PEAK);
    checkTrue("9am is off-peak", getTimeBand(9) == OFF_PEAK);
    checkTrue("12pm is off-peak", getTimeBand(12) == OFF_PEAK);
    checkTrue("6pm is peak", getTimeBand(18) == PEAK);
    checkTrue("7pm is peak", getTimeBand(19) == PEAK);
    checkTrue("8pm is off-peak", getTimeBand(20) == OFF_PEAK);

    cout << "\nGrab fares\n";
    Trip normal = makeTrip(10, 20, 14);
    checkMoney("GrabCar 10 km 20 min at 2pm", calculateGrabFare(normal, GRABCAR).total, 13.10);

    Trip peak = makeTrip(10, 20, 8);
    checkMoney("GrabCar same trip at 8am (same rates)", calculateGrabFare(peak, GRABCAR).total, 13.10);

    Trip jam = makeTrip(10, 45, 18);
    checkMoney("GrabCar 10 km stuck in traffic 45 min", calculateGrabFare(jam, GRABCAR).total, 23.85);

    Trip shortTrip = makeTrip(1, 3, 14);
    FareBreakdown shortFare = calculateGrabFare(shortTrip, GRABCAR);
    checkMoney("GrabCar 1 km 3 min uses minimum fare", shortFare.total, 5.00);
    checkTrue("Minimum fare flag is set", shortFare.minimumFareApplied);

    Trip busy = makeTrip(10, 20, 14);
    busy.demand = HIGH_DEMAND;
    checkMoney("GrabCar high demand (x1.3)", calculateGrabFare(busy, GRABCAR).total, 17.03);
    busy.demand = VERY_HIGH_DEMAND;
    checkMoney("GrabCar very high demand (x1.6)", calculateGrabFare(busy, GRABCAR).total, 20.96);

    checkMoney("GrabCar Premium 10 km 20 min", calculateGrabFare(normal, GRABCAR_PREMIUM).total, 29.00);
    Trip premiumShort = makeTrip(2, 5, 14);
    checkMoney("GrabCar Premium 2 km uses minimum fare", calculateGrabFare(premiumShort, GRABCAR_PREMIUM).total, 15.00);

    cout << "\nPromo codes, toll and cash rounding\n";
    Trip promo = makeTrip(12.5, 25, 14);                 // 2.00 + 3.13 + 10.75 = RM15.88
    promo.promoCode = "STUDENT10";
    checkMoney("STUDENT10 takes 10% off RM15.88", calculateGrabFare(promo, GRABCAR).discount, 1.59);
    checkMoney("STUDENT10 is capped at RM5", promoDiscount("STUDENT10", 80.00), 5.00);
    checkMoney("NEWRIDER takes RM5 off", promoDiscount("NEWRIDER", 13.00), 5.00);
    checkMoney("Discount never bigger than the fare", promoDiscount("NEWRIDER", 4.00), 4.00);
    checkTrue("Promo codes are not case-sensitive", isValidPromoCode("student10"));
    checkTrue("Unknown promo code is rejected", !isValidPromoCode("FREE"));
    checkMoney("Unknown promo code gives no discount", promoDiscount("FREE", 13.00), 0.00);

    Trip withToll = makeTrip(10, 20, 14);
    withToll.tollRM = 2.10;
    checkMoney("Toll is added to the fare", calculateGrabFare(withToll, GRABCAR).total, 15.20);

    promo.tollRM = 2.10;
    promo.payment = CASH;
    FareBreakdown cashFare = calculateGrabFare(promo, GRABCAR);
    checkMoney("Cash total RM16.39 rounded to RM16.40", cashFare.total, 16.40);
    double receiptSum = cashFare.baseFare + cashFare.distanceCharge + cashFare.timeCharge + cashFare.surgeOrSurcharge
                        - cashFare.discount + cashFare.toll + cashFare.roundingAdjustment;
    checkMoney("Receipt lines add up to the total", receiptSum, cashFare.total);
    checkMoney("Half a sen rounds up: RM3.125 -> RM3.13", roundToSen(3.125), 3.13);
    checkMoney("Half a sen rounds up: RM8.385 -> RM8.39", roundToSen(8.385), 8.39);
    checkMoney("Round RM16.77 -> RM16.75", roundToNearest5Sen(16.77), 16.75);
    checkMoney("Round RM16.78 -> RM16.80", roundToNearest5Sen(16.78), 16.80);
    checkMoney("Round RM16.72 -> RM16.70", roundToNearest5Sen(16.72), 16.70);
    checkMoney("Round RM16.73 -> RM16.75", roundToNearest5Sen(16.73), 16.75);

    cout << "\nTaxi fares\n";
    checkMoney("Budget taxi 10 km 20 min moderate traffic", calculateTaxiFare(normal, BUDGET_TAXI).total, 16.77);
    checkMoney("Teksi 1Malaysia 10 km 20 min", calculateTaxiFare(normal, TEKSI_1MALAYSIA).total, 20.50);

    Trip underOneKm = makeTrip(0.8, 20, 14);
    checkMoney("Budget taxi under 1 km pays flag fall only", calculateTaxiFare(underOneKm, BUDGET_TAXI).distanceCharge, 0.00);

    Trip lateNight = makeTrip(10, 20, 2);
    checkMoney("Budget taxi at 2am gets +50% surcharge", calculateTaxiFare(lateNight, BUDGET_TAXI).total, 25.16);

    Trip phone = makeTrip(10, 20, 14);
    phone.bookedByPhone = true;
    checkMoney("Budget taxi booked by phone (+RM2)", calculateTaxiFare(phone, BUDGET_TAXI).total, 18.77);

    Trip heavy = makeTrip(15, 30, 18);
    heavy.traffic = HEAVY_TRAFFIC;
    checkMoney("Budget taxi 15 km 30 min heavy traffic", calculateTaxiFare(heavy, BUDGET_TAXI).total, 26.80);

    Trip light = makeTrip(10, 20, 14);
    light.traffic = LIGHT_TRAFFIC;
    checkMoney("Budget taxi 10 km 20 min light traffic", calculateTaxiFare(light, BUDGET_TAXI).total, 15.09);

    cout << "\n" << testsPassed << " of " << testsRun << " tests passed.\n";
    return (testsPassed == testsRun) ? 0 : 1;
}
