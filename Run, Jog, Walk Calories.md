Level ground · No backpack

# Run, Jog, Walk Calories

Enter your weight, height and distance, then pick an activity. The estimate updates as you type.

## Your numbers

Sample values to start. Replace them with yours.

Weight

lb

Height

ft

in

5 ft 8 in = 68 in

Distance

miles

Activity · total for this distance

1 **Running**7 mph · 8:34 /mi **368**kcal 2 **Jogging**5 mph · 12:00 /mi **380**kcal 3 **Walking**3 mph · 20:00 /mi **260**kcal

Estimated total · Walking, 3.1 mi

260kcal

Activity **192.8** kcal Resting **67.6** kcal

Time

1:02:00

Pace

20:00 /mi

Formula

LCDA walking

BMI

22.8

Fat-free mass

107.2 lb

Resting rate

1,569 kcal/day

How it's calculated

## Resting part, all three activities

```
BMI            = 703 × lb ÷ in²
Fat-free mass  = lb × 9270 ÷ (7730 + 230 × BMI)
Resting rate   = 500 + 9.979 × fat-free mass       (kcal/day)
Resting kcal   = resting rate × hours ÷ 24
```

## Walking

```
fH            = (0.08196 + mph²/in) ÷ (0.08196 + mph²/68.5)
Walking kcal  = lb × hours × [0.07415 + fH × (0.4355 × mph^0.58 + 0.004208 × mph^4)]
```

## Running and jogging

```
Running kcal  = 0.730 × lb × miles
```

Hours are miles divided by mph. Walking uses the LCDA equation with a height factor, which is 1.0 at 68.5 in, the average height in the LCDA study. Running and jogging use the ACSM running equation, so their activity calories per mile don't depend on speed. Fat-free mass uses the Janmahasatian equation with the men's and women's coefficients averaged, so the result doesn't depend on sex.

The speeds are typical values: 7 mph and 5 mph sit in the middle of the usual running (6–8 mph) and jogging (4–6 mph) ranges, and 3 mph is the average adult walking speed. These are estimates for healthy adults. The walking model was tested on heights of 62–75 in.

## Sources

- [Looney et al. 2022, LCDA walking and load equation](https://pmc.ncbi.nlm.nih.gov/articles/PMC8919998/)
- [Janmahasatian et al., fat-free mass model](https://www.page-meeting.org/Abstracts/a-semi-mechanistic-model-for-quantification-of-lean-body-weight/)
- [Ludlow & Weyand 2016, walking cost and height](https://www.ideafit.com/walking-may-burn-more-calories-than-previously/)
- ACSM's Guidelines for Exercise Testing and Prescription, running equation
- Speeds: [walking](https://www.medicalnewstoday.com/articles/average-walking-speed), [jogging](https://www.healthline.com/health/average-jogging-speed), [running](https://www.newbalance.co.uk/running-advice/training/beginners-guides/running-vs-jogging/)