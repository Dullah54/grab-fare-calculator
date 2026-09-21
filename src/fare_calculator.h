// fare_calculator.h
// Data types and fare rates for the Grab Fare Calculator.
//
// The rates are simplified from publicly reported Klang Valley figures.
// See docs/design.md for where each rate comes from.

#ifndef FARE_CALCULATOR_H
#define FARE_CALCULATOR_H

#include <string>

// The four kinds of ride the program can price
enum RideType {
    GRABCAR = 1,
    GRABCAR_PREMIUM = 2,
    BUDGET_TAXI = 3,
    TEKSI_1MALAYSIA = 4
};

// Time of day decides which rates and surcharges apply
enum TimeBand {
    OFF_PEAK,
    PEAK,       // 7:00-9:59am and 5:00-7:59pm
    MIDNIGHT    // 12:00am-5:59am
};

enum TrafficLevel  { LIGHT_TRAFFIC = 1, MODERATE_TRAFFIC = 2, HEAVY_TRAFFIC = 3 };
enum DemandLevel   { NORMAL_DEMAND = 1, HIGH_DEMAND = 2, VERY_HIGH_DEMAND = 3 };
enum PaymentMethod { GRABPAY = 1, CASH = 2 };

// Everything the user tells us about one trip
struct Trip {
    double distanceKm = 0.0;
    int durationMin = 0;
    int pickupHour = 12;
    TrafficLevel traffic = MODERATE_TRAFFIC;
    DemandLevel demand = NORMAL_DEMAND;
    double tollRM = 0.0;
    std::string promoCode;          // Grab only, may be empty
    PaymentMethod payment = GRABPAY;
    bool bookedByPhone = false;     // taxi only
};

// Each part of a fare, kept separately so we can print a clear receipt
struct FareBreakdown {
    RideType rideType = GRABCAR;
    double baseFare = 0.0;
    double ratePerKm = 0.0;
    double ratePerMin = 0.0;
    double chargedKm = 0.0;         // taxi flag fall already covers the first km
    double chargedMinutes = 0.0;    // taxi meter only charges waiting time
    double distanceCharge = 0.0;
    double timeCharge = 0.0;
    double surgeOrSurcharge = 0.0;  // Grab demand surge or taxi midnight surcharge
    bool minimumFareApplied = false;
    double bookingFee = 0.0;
    double discount = 0.0;
    double toll = 0.0;
    double roundingAdjustment = 0.0;
    double total = 0.0;
};

// ---------- Grab rates (Klang Valley) ----------
const double GRABCAR_BASE            = 2.00;
const double GRABCAR_PER_KM          = 0.70;
const double GRABCAR_PER_MIN         = 0.20;
const double GRABCAR_PEAK_PER_KM     = 0.25;   // peak-hour rates since January 2023
const double GRABCAR_PEAK_PER_MIN    = 0.43;
const double GRABCAR_MINIMUM         = 5.00;

const double PREMIUM_BASE            = 4.00;
const double PREMIUM_PER_KM          = 1.50;
const double PREMIUM_PER_MIN         = 0.50;
const double PREMIUM_MINIMUM         = 15.00;

// Simplified dynamic pricing (surge) multipliers
const double SURGE_NORMAL            = 1.0;
const double SURGE_HIGH              = 1.3;
const double SURGE_VERY_HIGH         = 1.6;

// ---------- Taxi meter rates (SPAD, 2015) ----------
const double BUDGET_TAXI_FLAG_FALL   = 3.00;
const double BUDGET_TAXI_PER_KM      = 1.25;
const double BUDGET_TAXI_PER_MIN     = 0.42;

const double TEKSI1M_FLAG_FALL       = 4.00;
const double TEKSI1M_PER_KM          = 1.50;
const double TEKSI1M_PER_MIN         = 0.50;

const double FLAG_FALL_COVERS_KM     = 1.0;    // flag fall includes the first kilometre
const double MIDNIGHT_SURCHARGE_RATE = 0.50;   // +50% from 12:00am to 5:59am
const double PHONE_BOOKING_FEE       = 2.00;

// Share of the trip time the taxi meter charges as waiting time
const double WAITING_SHARE_LIGHT     = 0.10;
const double WAITING_SHARE_MODERATE  = 0.30;
const double WAITING_SHARE_HEAVY     = 0.50;

#endif
