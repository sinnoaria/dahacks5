#pragma once

#include <iostream>
#include <string>
#include <time.h>
#include "FitnessResults.h"
using namespace std;


double calculateCalories(double weight, double distance);
double calculateDistance(double weight, double calories);
void showBMI(double heightInches, double weightPounds);

double calculateCalories(double weight, double distance) {
    const double CalorieBurnRate = 0.76;
    // Assuming distance is in miles and weight is in pounds
    double caloriesBurned = CalorieBurnRate * weight * distance;
    std::cout << "Calories burned: " << caloriesBurned << endl;
    return caloriesBurned;
}

double calculateDistance(double weight, double calories) {
    const double CalorieBurnRate = 0.76;
    // Assuming weight is in pounds and calories is the total calories burned
    double distance = calories / (CalorieBurnRate * weight);
    std::cout << "Distance needed: " << distance << " miles" << endl;
    return distance;
}

void showBMI(double heightInches, double weightPounds) {
    // BMI = (weight in pounds / (height in inches * height in inches)) * 703
    double bmi = (weightPounds / (heightInches * heightInches)) * 703;
    std::cout << "Your BMI is: " << bmi << endl;
}