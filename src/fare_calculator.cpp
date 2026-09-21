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

// Works out the time band from the pickup hour (0-23)
TimeBand getTimeBand(int hour) {
    if (hour >= 0 && hour <= 5) {
        return MIDNIGHT;
    } else if ((hour >= 7 && hour <= 9) || (hour >= 17 && hour <= 19)) {
        return PEAK;
    }
    return OFF_PEAK;
}

// Returns a readable name for a time band
std::string timeBandName(TimeBand band) {
    switch (band) {
        case PEAK:     return "Peak hours";
        case MIDNIGHT: return "Midnight (12am-6am)";
        default:       return "Off-peak";
    }
}

// Simplified dynamic pricing: busier times cost more on Grab
double surgeMultiplier(DemandLevel demand) {
    switch (demand) {
        case HIGH_DEMAND:      return SURGE_HIGH;
        case VERY_HIGH_DEMAND: return SURGE_VERY_HIGH;
        default:               return SURGE_NORMAL;
    }
}

// Grab fare = (base + distance + time) x surge, but never less than the minimum fare
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
        fare.baseFare = GRABCAR_BASE;
        minimumFare   = GRABCAR_MINIMUM;

        // Since January 2023 GrabCar charges more per minute and less per km
        // during peak hours, because traffic jams make trips take longer
        switch (getTimeBand(trip.pickupHour)) {
            case PEAK:
                fare.ratePerKm  = GRABCAR_PEAK_PER_KM;
                fare.ratePerMin = GRABCAR_PEAK_PER_MIN;
                break;
            case OFF_PEAK:
            case MIDNIGHT:
                fare.ratePerKm  = GRABCAR_PER_KM;
                fare.ratePerMin = GRABCAR_PER_MIN;
                break;
        }
    }

    fare.chargedKm      = trip.distanceKm;
    fare.chargedMinutes = trip.durationMin;
    fare.distanceCharge = fare.chargedKm * fare.ratePerKm;
    fare.timeCharge     = fare.chargedMinutes * fare.ratePerMin;

    double rideFare = fare.baseFare + fare.distanceCharge + fare.timeCharge;

    // Demand surge is added on top of the normal fare
    fare.surgeOrSurcharge = rideFare * (surgeMultiplier(trip.demand) - 1.0);
    rideFare += fare.surgeOrSurcharge;

    if (rideFare < minimumFare) {
        rideFare = minimumFare;
        fare.minimumFareApplied = true;
    }

    fare.total = roundToSen(rideFare);
    return fare;
}
