// fare_calculator.cpp
// Fare calculation logic for the Grab Fare Calculator.
// No input or output happens here, so every function can be tested on its own.

#include "fare_calculator.h"

#include <cmath>

// Rounds a ringgit amount to 2 decimal places (the nearest sen)
double roundToSen(double amount) {
    return std::round(amount * 100.0) / 100.0;
}

// Returns a readable name for a ride type
std::string rideTypeName(RideType type) {
    switch (type) {
        case GRABCAR:         return "GrabCar";
        case GRABCAR_PREMIUM: return "GrabCar Premium";
        case BUDGET_TAXI:     return "Budget taxi";
        case TEKSI_1MALAYSIA: return "Teksi 1Malaysia";
    }
    return "Unknown";
}

// Grab fare = base + distance + time, but never less than the minimum fare
FareBreakdown calculateGrabFare(const Trip& trip, RideType type) {
    FareBreakdown fare;
    fare.rideType = type;

    double minimumFare = 0.0;
    if (type == GRABCAR_PREMIUM) {
        fare.baseFare   = PREMIUM_BASE;
        fare.ratePerKm  = PREMIUM_PER_KM;
        fare.ratePerMin = PREMIUM_PER_MIN;
        minimumFare     = PREMIUM_MINIMUM;
    } else {
        fare.baseFare   = GRABCAR_BASE;
        fare.ratePerKm  = GRABCAR_PER_KM;
        fare.ratePerMin = GRABCAR_PER_MIN;
        minimumFare     = GRABCAR_MINIMUM;
    }

    fare.chargedKm      = trip.distanceKm;
    fare.chargedMinutes = trip.durationMin;
    fare.distanceCharge = fare.chargedKm * fare.ratePerKm;
    fare.timeCharge     = fare.chargedMinutes * fare.ratePerMin;

    double rideFare = fare.baseFare + fare.distanceCharge + fare.timeCharge;
    if (rideFare < minimumFare) {
        rideFare = minimumFare;
        fare.minimumFareApplied = true;
    }

    fare.total = roundToSen(rideFare);
    return fare;
}
