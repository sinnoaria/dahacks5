#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
#include <vector>
#include "modules/FitnessResults.h"
#include "modules/fitnessUserInput.h"
#include "modules/fitnessCalculations.h"
#include "modules/fitnessGui.h"

using namespace std; //include all std for simplicity



int main() {
    /*
    pathType (tbd) - assume concrete for now
    movementType (tbd) - assume walking for now
    Age (tbd)
    */
    double heightInches = 0.0;
    double weightPounds = 0.0;
    int choice = 0;
    double distance = 0.0;
    double calories = 0.0;
    vector<FitnessResults> results;
    bool continueInput = true;

    do {

        getUserInput(heightInches, weightPounds, choice, distance, calories);
        if (choice == 1) {
            // Calculate calories burned based on weight and distance
            calories =calculateCalories(weightPounds, distance); // Example distance of 5 mi
        } else if (choice == 2) {
            // Calculate distance based on weight and calories burned
            distance = calculateDistance(weightPounds, calories); // Example calories burned of 100
        } else {
            cout << "Something went wrong" << endl;
        }
        showBMI(heightInches, weightPounds);
        results.emplace_back();
        results.back().setHeightInches(heightInches);
        results.back().setWeightPounds(weightPounds);
        results.back().setTimestamp(time(nullptr));
        results.back().setCalculatedBMI();
        results.back().setDistanceMiles(distance);
        results.back().setCaloriesBurned(calories);
        results.back().displayResults();

        ofstream outFile("fitness_results.txt", ios::app); // Open the file in append mode
        if (outFile.is_open()) {
            {
                outFile << "Height: " << results.back().getHeightInches() << " inches" << endl;
                outFile << "Weight: " << results.back().getWeightPounds() << " pounds" << endl;
                outFile << "BMI: " << results.back().getBMI() << endl;
                outFile << "Distance: " << results.back().getDistanceMiles() << " miles" << endl;
                outFile << "Calories Burned: " << results.back().getCaloriesBurned() << endl;
                outFile << "Timestamp: " << results.back().getTimestamp() << endl;
                outFile << endl; // Add a blank line between entries
            }
            outFile.close();
            cout << "Results saved to fitness_results.txt" << endl;
        } else {
            cout << "Unable to open file for writing." << endl;
        }
        cout << "Do you want to enter another set of data? (y/n): ";
        char userChoice;
        cin >> userChoice;
        clearCin();
        if (userChoice == 'n' || userChoice == 'N') {
            continueInput = false;
        }
    } while (continueInput);


    return 0;

}

