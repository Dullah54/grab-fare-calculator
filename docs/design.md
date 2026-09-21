# Program Design - Grab Fare Calculator

## 1. Purpose

Estimate the fare of a ride-hailing trip in the Klang Valley and compare it with a traditional
metered taxi. This links directly to Part 1 (Grab as a disruptive innovation): lower and upfront
prices were one of the main reasons passengers moved from taxis to Grab.

## 2. Inputs

| Input | Type | Valid range | Used for |
|---|---|---|---|
| Menu choice | integer | 0 - 5 | choose a feature |
| Trip distance | decimal (km) | 0.5 - 300 | distance charge |
| Trip duration | integer (minutes) | 1 - 600 | time charge |
| Pickup hour | integer | 0 - 23 | peak / midnight rules |
| Traffic level | integer | 1 light, 2 moderate, 3 heavy | taxi waiting time |
| Demand level | integer | 1 normal, 2 high, 3 very high | Grab surge |
| Ride type | integer | 1 GrabCar, 2 GrabCar Premium | Grab rates |
| Toll charges | decimal (RM) | 0 - 100 | added to the fare |
| Promo code | text | optional | Grab discount |
| Payment method | integer | 1 GrabPay / card, 2 cash | cash rounding |
| Taxi booked by phone? | y / n | - | RM2 booking fee |

## 3. Outputs

- Fare breakdown (base fare, distance, time, surge or surcharge, discount, toll, total).
- A comparison table of GrabCar, GrabCar Premium, budget taxi and Teksi 1Malaysia for the same trip,
  showing the cheapest option and how much the passenger saves compared with a budget taxi.
- A rate card listing all the rates used.
- A short summary of how Grab disrupted the taxi industry (from Part 1).
- A history of the trips estimated in the current session, with a summary on exit.
- Clear error messages when an input is not valid, then the question is asked again.

## 4. Fare rules

### Grab (ride-hailing)

`fare = (base + km x rate per km + minutes x rate per minute) x demand multiplier`,
never lower than the minimum fare, then minus promo discount, plus toll.

| | Base | Per km | Per minute | Minimum |
|---|---|---|---|---|
| GrabCar (off-peak) | RM2.00 | RM0.70 | RM0.20 | RM5.00 |
| GrabCar (peak hours) | RM2.00 | RM0.25 | RM0.43 | RM5.00 |
| GrabCar Premium | RM4.00 | RM1.50 | RM0.50 | RM15.00 |

- GrabCar rates and the January 2023 peak-hour change: RinggitPlus (Cheong, 2023).
- Premium rates are estimates from public fare estimators.
- Demand multiplier (dynamic pricing) is simplified for this program: normal x1.0, high x1.3, very high x1.6.
- Peak hours used in this program: 7:00-9:59am and 5:00-7:59pm.

### Traditional taxi (metered)

`fare = flag fall (covers first km) + extra km x rate per km + waiting minutes x rate per minute`,
plus 50% between 12:00am and 5:59am, plus RM2 if booked by phone, plus toll.

| | Flag fall | Per km | Per minute (waiting) |
|---|---|---|---|
| Budget taxi | RM3.00 | RM1.25 | RM0.42 |
| Teksi 1Malaysia | RM4.00 | RM1.50 | RM0.50 |

- Klang Valley meter rates set by SPAD in 2015 (The Edge Malaysia, 2015).
- A meter only charges time while the taxi is moving slowly, so the program charges waiting time for
  part of the trip: 10% (light traffic), 30% (moderate) or 50% (heavy).

### Cash rounding

Cash totals are rounded to the nearest 5 sen, following Bank Negara Malaysia's rounding mechanism.

## 5. Logic plan

- `switch` on the main menu choice.
- `switch` on the time band (off-peak, peak, midnight) to pick rates and surcharges.
- `if / else` for ride types, minimum fare, promo codes, booking fee and the cheapest option.
- Input helper functions that keep asking until the value is valid.
