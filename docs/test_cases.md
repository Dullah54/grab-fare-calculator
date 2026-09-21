# Test Cases

## 1. Automated tests (`tests/test_fare_calculator.cpp`)

43 tests check the fare functions against answers worked out by hand. Result: **43 of 43 passed**.

| Area | What is tested | Example | Expected |
|---|---|---|---|
| Time bands | 12am, 5am, 6am, 8am, 9am, 12pm, 6pm, 7pm, 8pm | 8am | Peak |
| Grab fare | 10 km, 20 min (same at 2pm and 8am) | 2.00 + 2.50 + 8.60 | RM13.10 |
| Traffic jam | 10 km, 45 min | 2.00 + 2.50 + 19.35 | RM23.85 |
| Minimum fare | GrabCar 1 km, 3 min | 3.54 is below minimum | RM5.00 |
| Surge | high demand x1.3 on RM13.10 | 13.10 x 1.3 | RM17.03 |
| Premium | 10 km, 20 min | 4.00 + 15.00 + 10.00 | RM29.00 |
| Promo | STUDENT10 on RM15.88 (2.00 + 3.13 + 10.75) | 10% | RM1.59 off |
| Promo | STUDENT10 on RM80 | capped | RM5.00 off |
| Promo | invalid code FREE | rejected | RM0.00 off |
| Toll | RM2.10 toll on RM13.10 | 13.10 + 2.10 | RM15.20 |
| Cash rounding | RM16.39 paid in cash | nearest 5 sen | RM16.40 |
| Receipt | base + distance + time - discount + toll + rounding | adds up to the total | RM16.40 |
| Toll rounding | RM1.005 toll on RM13.10, paid in cash | toll rounded to RM1.01, receipt adds up | RM14.10 |
| Half-sen rounding | RM3.125 and RM8.385 | round up | RM3.13, RM8.39 |
| Budget taxi | 10 km, 20 min, moderate traffic | 3.00 + 11.25 + 2.52 | RM16.77 |
| Teksi 1Malaysia | 10 km, 20 min | 4.00 + 13.50 + 3.00 | RM20.50 |
| Midnight | budget taxi at 2am | 16.77 x 1.5 | RM25.16 |
| Phone booking | budget taxi booked by phone | 16.77 + 2.00 | RM18.77 |
| Heavy traffic | 15 km, 30 min at 6pm | 3.00 + 17.50 + 6.30 | RM26.80 |

## 2. Manual tests (running the program)

| # | Test | Input | Expected result | Actual result |
|---|---|---|---|---|
| M1 | Letters at the menu | `abc` | Error message, asks again | Pass |
| M2 | Menu number out of range | `9` | Error message, asks again | Pass |
| M3 | History before any trip | `5` | "No trips yet" message | Pass |
| M4 | Negative distance | `-5` | Error, asks again | Pass |
| M5 | Zero distance | `0` | Error (minimum 0.5 km) | Pass |
| M6 | Distance typed as a word | `twelve` | Error, asks again | Pass |
| M7 | Hour out of range | `25` | Error (0 - 23) | Pass |
| M8 | Toll typed as letters | `abc` | Error, asks again | Pass |
| M9 | Demand level out of range | `4` | Error (1 - 3) | Pass |
| M10 | Ride type out of range | `3` | Error (1 - 2) | Pass |
| M11 | Invalid promo code | `FREE50` | "not a valid promo code", asks again | Pass |
| M12 | Skip promo code | Enter | No discount applied | Pass |
| M13 | Payment choice out of range | `3` | Error (1 - 2) | Pass |
| M14 | Unclear yes/no answer | `maybe` | "Please answer y or n" | Pass |
| M15 | Normal comparison, peak hour | 15 km, 30 min, 6pm | GrabCar cheapest, saves RM8.15 | Pass |
| M16 | Comparison with very high surge | 15 km, 30 min, 6pm, surge x1.6 | Budget taxi cheaper, message explains surge | Pass |
| M17 | Exit after trips | `0` | Session summary, then goodbye | Pass |
| M18 | Input ends suddenly (Ctrl+Z) | end of input | Program exits safely | Pass |
