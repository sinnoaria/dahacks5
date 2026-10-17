#pragma once

#include <iostream>
#include <string>
#include <time.h>
#include <vector>
#include "cinErrorHandling.h"
#include "FitnessResults.h"

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::vector;

double getUserHeight();
double getUserWeight();
double getUserDistance();
double getUserCalories();
int getChoice();
int getPathType();
int getMovementType();

void getUserInput(
    double& heightInches,
    double& weightPounds,
    int& choice,
    double& distance,
    double& calories
);

void getUserInput(vector<FitnessResults>& results, int& choice) {
    FitnessResults result;
    result.setHeightInches(getUserHeight());
    result.setWeightPounds(getUserWeight());
    result.setTimestamp(time(nullptr));
    choice = getChoice();

    if (choice == 1) {
        result.setDistanceMiles(getUserDistance());
        result.calculateCaloriesBurned();
    } else if (choice == 2) {
        result.setCaloriesBurned(getUserCalories());
        result.calculateDistanceForCalories();
    }

    result.calculateBMI();
    results.push_back(result);
}



double getUserHeight() {
    double heightInches;
    do {
        cout << "Enter your height in inches: ";
        cin >> heightInches;
        clearCin(heightInches, 0.0);
    } while (heightInches <= 0);
    return heightInches;
}

double getUserWeight() {
    double weightPounds;
    do {
        cout << "Enter your weight in pounds: ";
        cin >> weightPounds;
        clearCin(weightPounds, 0.0);
    } while (weightPounds <= 0);
    return weightPounds;
}

double getUserDistance() {
    double distance;
    do {
        cout << "Enter the distance in miles: ";
        cin >> distance;
        clearCin(distance, 0.0);
    } while (distance <= 0);
    return distance;
}

double getUserCalories() {
    double calories;
    do {
        cout << "Enter the calories burned: ";
        cin >> calories;
        clearCin(calories, 0.0);
    } while (calories <= 0);
    return calories;
}

int getChoice() {
    string choice = "";
    int choiceInt = 0;
    do {
        cout << "Do you want to calculate \n" <<
            "1. [calories] burned by distance or\n" <<
            "2. [distance] for a certain distance?\n" << 
            "(Enter \'1\' or \'calories\' or \'2\' or \'distance\'): ";
        cin >> choice;
        clearCin();
        if (choice == "1" || choice == "calories") {
            choiceInt = 1;
        } else if (choice == "2" || choice == "distance") {
            choiceInt = 2;
        }
        if (choiceInt != 1 && choiceInt != 2) {
            cout << "Invalid choice. Please enter \'1\' " <<
            "or \'calories\' to calculate the calories burned " <<
            "or \'2\' or \'distance\' to calculate the distance needed." << 
            endl;
        }
    } while (choiceInt != 1 && choiceInt != 2);
    return choiceInt;
}

int getPathType() {
    int pathType = 0;
    string choice = "";
    do {
        cout << "Enter the path type (1 for concrete, 2 for dirt): ";//, 3 for grass): ";
        cin >> choice;
        clearCin();
        if (choice == "1" || choice == "concrete") {
            pathType = 1;
        } else if (choice == "2" || choice == "dirt") {
            pathType = 2;
        } else if (choice == "3" || choice == "grass") {
            pathType = 3;
        }
        if (pathType < 1 || pathType > 2) {
            cout << "Invalid path type. Please enter \'1\', \'concrete\', \'2\', or \'dirt\'." << endl;
        }
    } while (pathType < 1 || pathType > 2);
    return pathType;
}

int getMovementType() {
    int movementType = 0;
    string choice = "";
    do {
        cout << "Enter the movement type (1 for walking, 2 for running): ";
        cin >> choice;
        clearCin();
        if (choice == "1" || choice == "walking") {
            movementType = 1;
        } else if (choice == "2" || choice == "running") {
            movementType = 2;
        }
        if (movementType < 1 || movementType > 2) {
            cout << "Invalid movement type. Please enter \'1\', \'walking\', \'2\', or \'running\'." << endl;
        }
    } while (movementType < 1 || movementType > 2);
    return movementType;
}

void getUserInput(double& heightInches, double& weightPounds, int& choice, double& distance, double& calories) {
    heightInches = getUserHeight();
    weightPounds = getUserWeight();
    choice = getChoice();
    if (choice == 1) {
        distance = getUserDistance();
    } else if (choice == 2) {
        calories = getUserCalories();
    }
    
}