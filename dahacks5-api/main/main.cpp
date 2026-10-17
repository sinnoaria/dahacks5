#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
#include <vector>
#include "modules/FitnessResults.h"
#include "modules/fitnessUserInput.h"
#include "modules/fitnessCalculations.h"
#include "modules/fitnessGui.cpp"

using std::cout;
using std::endl;
using std::string;
using std::vector;

// One calculation for the web page: read the four answers from stdin, print the
// lines the page parses, and exit. server.ps1 builds this file and runs it with
// --cli; without arguments the program still opens the normal GUI.
int runConsoleOnce() {
    double heightInches = 0.0;
    double weightPounds = 0.0;
    int choice = 0;
    double distance = 0.0;
    double calories = 0.0;

    getUserInput(heightInches, weightPounds, choice, distance, calories);
    if (choice == 1) {
        // Calculate calories burned based on weight and distance
        calories = calculateCalories(weightPounds, distance);
    } else if (choice == 2) {
        // Calculate distance based on weight and calories burned
        distance = calculateDistance(weightPounds, calories);
    } else {
        cout << "Something went wrong" << endl;
    }

    FitnessResults result;
    result.setHeightInches(heightInches);
    result.setWeightPounds(weightPounds);
    result.setTimestamp(time(nullptr));
    result.setCalculatedBMI();
    result.setDistanceMiles(distance);
    result.setCaloriesBurned(calories);
    result.displayResults();

    // The page asks for a new result on every edit, so a web run is not
    // appended to fitness_results.txt. Use the GUI's "Save Results" button
    // when a result should be kept.
    return 0;
}

int main(int argc, char** argv) {
    /*
    pathType (tbd) - assume concrete for now
    movementType (tbd) - assume walking for now
    Age (tbd)
    */
    for (int index = 1; index < argc; ++index) {
        string argument = argv[index];
        if (argument == "--cli" || argument == "--once") {
            return runConsoleOnce();
        }
    }

    vector<FitnessResults> results;
    runFitnessGui(results);
    return 0;
}
