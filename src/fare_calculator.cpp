// fare_calculator.cpp
// Fare calculation logic for the Grab Fare Calculator.
// No input or output happens here, so every function can be tested on its own.

#include "fare_calculator.h"

#include <cctype>
#include <cmath>

// Rounds a ringgit amount to 2 decimal places (the nearest sen)
double roundToSen(double amount) {
    return std::round(amount * 100.0) / 100.0;
}

// Cash payments in Malaysia are rounded to the nearest 5 sen
// (e.g. RM16.77 -> RM16.75, RM16.78 -> RM16.80)
double roundToNearest5Sen(double amount) {
    return std::round(amount * 20.0) / 20.0;
}

// Converts text to upper case so promo codes are not case-sensitive
std::string toUpperCase(std::string text) {
    for (char& c : text) {
        c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    }
    return text;
}

// Sample promo codes used in this program (they are not real Grab codes):
//   STUDENT10 - 10% off, up to RM5
//   NEWRIDER  - RM5 off
bool isValidPromoCode(const std::string& code) {
    std::string upper = toUpperCase(code);
    return upper == "STUDENT10" || upper == "NEWRIDER";
}

// Returns how much the promo code takes off the fare (0 if the code is not valid)
double promoDiscount(const std::string& code, double fare) {
    std::string upper = toUpperCase(code);
    double discount = 0.0;

    if (upper == "STUDENT10") {
        discount = fare * 0.10;
        if (discount > 5.00) {
            discount = 5.00;
        }
    } else if (upper == "NEWRIDER") {
        discount = 5.00;
    }

    // A discount can never be bigger than the fare, so the total is never negative
    if (discount > fare) {
        discount = fare;
    }
    return roundToSen(discount);
}

// Adds toll and applies cash rounding - the last step for every fare
void finishFare(FareBreakdown& fare, double fareBeforeToll, const Trip& trip) {
    fare.toll = trip.tollRM;
    double total = roundToSen(fareBeforeToll + fare.toll);

    if (trip.payment == CASH) {
        double rounded = roundToNearest5Sen(total);
        fare.roundingAdjustment = roundToSen(rounded - total);
        total = rounded;
    }
    fare.total = total;
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

// Works out the time band from the pickup hour (0-23).
// Peak hours are Grab's listed peak hours: 7-9am and 5-8pm.
TimeBand getTimeBand(int hour) {
    if (hour >= 0 && hour <= 5) {
        return MIDNIGHT;
    } else if ((hour >= 7 && hour <= 8) || (hour >= 17 && hour <= 19)) {
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
        // GrabCar uses the same rates all day; a trip in peak-hour traffic
        // costs more because it takes more minutes
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

    // Demand surge is added on top of the normal fare
    fare.surgeOrSurcharge = rideFare * (surgeMultiplier(trip.demand) - 1.0);
    rideFare += fare.surgeOrSurcharge;

    if (rideFare < minimumFare) {
        rideFare = minimumFare;
        fare.minimumFareApplied = true;
    }

    // Promo codes only apply to Grab rides
    fare.discount = promoDiscount(trip.promoCode, rideFare);
    rideFare -= fare.discount;

    finishFare(fare, rideFare, trip);
    return fare;
}

// True for the two taxi ride types
bool isTaxi(RideType type) {
    return type == BUDGET_TAXI || type == TEKSI_1MALAYSIA;
}

// A taxi meter only charges for time while the taxi is stuck or moving slowly,
// so we charge waiting time for part of the trip depending on the traffic
double waitingShare(TrafficLevel traffic) {
    switch (traffic) {
        case LIGHT_TRAFFIC: return WAITING_SHARE_LIGHT;
        case HEAVY_TRAFFIC: return WAITING_SHARE_HEAVY;
        default:            return WAITING_SHARE_MODERATE;
    }
}

// Taxi fare = flag fall (covers the first km) + extra km + waiting time
FareBreakdown calculateTaxiFare(const Trip& trip, RideType type) {
    FareBreakdown fare;
    fare.rideType = type;

    if (type == TEKSI_1MALAYSIA) {
        fare.baseFare   = TEKSI1M_FLAG_FALL;
        fare.ratePerKm  = TEKSI1M_PER_KM;
        fare.ratePerMin = TEKSI1M_PER_MIN;
    } else {
        fare.baseFare   = BUDGET_TAXI_FLAG_FALL;
        fare.ratePerKm  = BUDGET_TAXI_PER_KM;
        fare.ratePerMin = BUDGET_TAXI_PER_MIN;
    }

    // The flag fall already pays for the first kilometre
    fare.chargedKm = trip.distanceKm - FLAG_FALL_COVERS_KM;
    if (fare.chargedKm < 0) {
        fare.chargedKm = 0;
    }
    fare.chargedMinutes = trip.durationMin * waitingShare(trip.traffic);
    fare.distanceCharge = fare.chargedKm * fare.ratePerKm;
    fare.timeCharge     = fare.chargedMinutes * fare.ratePerMin;

    double meterFare = fare.baseFare + fare.distanceCharge + fare.timeCharge;

    // Taxis charge 50% extra between midnight and 6am
    switch (getTimeBand(trip.pickupHour)) {
        case MIDNIGHT:
            fare.surgeOrSurcharge = meterFare * MIDNIGHT_SURCHARGE_RATE;
            break;
        case PEAK:
        case OFF_PEAK:
            fare.surgeOrSurcharge = 0.0;
            break;
    }

    // Booking a taxi by phone costs an extra RM2
    if (trip.bookedByPhone) {
        fare.bookingFee = PHONE_BOOKING_FEE;
    }

    finishFare(fare, meterFare + fare.surgeOrSurcharge + fare.bookingFee, trip);
    return fare;
}
