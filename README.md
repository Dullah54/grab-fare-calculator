# Grab Fare Calculator (C++)

LDCW6123 Fundamentals of Digital Competence for Programmer - Group Project, Part 2.

## Why this program?

In Part 1 of our project we traced how **Grab** (which started as MyTeksi in Kuala Lumpur in 2012)
disrupted the traditional taxi industry, using Clayton Christensen's disruptive innovation model.
One of the main reasons passengers switched from taxis to Grab was price and price transparency:
Grab shows an upfront fare before the trip, while a taxi meter only tells you the price at the end.

This program recreates that idea. The user enters the details of a trip in the Klang Valley and the
program estimates the Grab fare and compares it with a traditional metered taxi.

## Features

| Menu | Feature |
|---|---|
| 1 | Estimate a GrabCar or GrabCar Premium fare, with demand surge, promo codes, toll and cash rounding |
| 2 | Compare GrabCar, GrabCar Premium, a budget taxi and Teksi 1Malaysia for the same trip and show the cheapest |
| 3 | Rate card with every rate the program uses |
| 4 | Short summary of Grab's disruption of the taxi industry (links to our Part 1 poster) |
| 5 | History of the trips estimated in this session |
| 0 | Exit with a session summary |

Every input is checked. Letters, empty answers and numbers outside the allowed range show a message
and the question is asked again, so the program never crashes on bad input.

## Inputs and outputs

- **Inputs:** trip distance (km), trip time (minutes), pickup hour, toll, traffic level, demand level,
  ride type, promo code, payment method, and whether a taxi is booked by phone.
- **Outputs:** a fare breakdown (base fare, distance, time, surge or surcharge, discount, toll, total),
  a comparison table with the cheapest option and the saving, and a session summary.

See [docs/design.md](docs/design.md) for the full input/output plan and the fare rules.

## How to build and run

You need a C++17 compiler such as g++ (MinGW on Windows).

```
g++ -std=c++17 -Wall -Wextra -o grab_fare src/main.cpp src/fare_calculator.cpp
./grab_fare
```

On Windows you can also double-click or run `build.bat`, then run `grab_fare.exe`.

## Tests

```
g++ -std=c++17 -Wall -Wextra -Isrc -o run_tests tests/test_fare_calculator.cpp src/fare_calculator.cpp
./run_tests
```

43 automated tests check the fare calculations (43 of 43 pass). The manual tests for invalid input
are listed in [docs/test_cases.md](docs/test_cases.md).

## Sample run

```
--- COMPARE GRAB WITH A TRADITIONAL TAXI ---
Trip distance in km (0.5 - 300): 15
Estimated trip time in minutes (1 - 600): 30
Pickup hour in 24-hour time (0 - 23, e.g. 8 = 8am, 18 = 6pm): 18
  -> Time band: Peak hours
...
  ------------------------------------------------------
  Ride option         Estimated fare  Pricing
  ------------------------------------------------------
  GrabCar             RM   18.65    upfront (known before booking)  <- cheapest
  GrabCar Premium     RM   41.50    upfront (known before booking)
  Budget taxi         RM   26.80    meter (known at the end)
  Teksi 1Malaysia     RM   32.50    meter (known at the end)
  ------------------------------------------------------
  GrabCar saves you RM8.15 (30%) compared with a budget taxi.
```

## Project structure

```
src/fare_calculator.h     data types, fare rates and function declarations
src/fare_calculator.cpp   fare calculation logic (no input/output, so it can be tested)
src/main.cpp              menu, input validation and screens
tests/                    automated tests
docs/design.md            input/output plan and fare rules
docs/test_cases.md        automated and manual test cases
build.bat                 Windows build script
```

## About the rates

The program uses published rates, simplified for learning purposes. They are not official Grab
or taxi price quotes.

- GrabCar: RM0.25/km and RM0.43/min in the Klang Valley since 16 January 2023 (before that RM0.70/km and
  RM0.20/min). Yeoh, A. (2023, January 16). Grab updates fare structure, rides during peak hours to cost more.
  *The Star*. https://www.thestar.com.my/tech/tech-news/2023/01/16/grab-updates-fee-structure-rides-during-peak-hours-to-cost-more
- GrabCar's RM2 base fare and RM5 minimum fare (unchanged in 2023) and Grab's peak hours (7-9am, 5-8pm):
  Dzafri, D. (2023, January 17). Grab Malaysia raises fare prices per minute. This is why. *SoyaCincau*.
  https://soyacincau.com/2023/01/17/grab-malaysia-raises-fare-prices-per-minute-this-is-why/
- GrabCar Premium: TaxiFareFinder. (n.d.). *GrabCar Premium price estimator & cost calculator: Kuala Lumpur,
  Malaysia*. https://www.taxifarefinder.com/main.php?city=GrabCar-Premium-Kuala-Lumpur-Malaysia
- Budget taxi and Teksi 1Malaysia meter rates set by SPAD in 2015 (the RM3 flag fall covers the first km):
  Meng, Y. C., Lee, P., & Gasper, D. T. (2015, March 20). Fares increased for taxi, train and express bus
  services. *The Star*; The Edge Malaysia. (2015, March 19). Taxi fares go up immediately, express bus from
  May 15 – SPAD.
- Midnight surcharge and phone booking fee: klia2.info. (n.d.). *Useful tips on using Malaysian taxi services*.
  https://www.klia2.info/taxis/useful-tips-on-using-malaysian-taxi-services/
- Cash rounding to the nearest 5 sen (since 1 April 2008): Bank Negara Malaysia. (2007, November 13).
  *Introduction of a rounding mechanism for over-the-counter payments* [Press release].
  https://www.bnm.gov.my/-/introduction-of-a-rounding-mechanism-for-over-the-counter-payments
- Our own simplifications: the surge levels (x1.3, x1.6), the share of taxi time charged as waiting
  time, and the promo codes STUDENT10 and NEWRIDER.
