
#include <iostream>
#include <string>
using namespace std; //include all std for simplicity

void getUserInput(
    double& heightInches,
    double& weightPounds,
    string& choice,
    double& distance,
    double& calories
);

void calculateCalories(double weight, double distance);
void calculateDistance(double weight, double calories);


int main() {
    /*
    Weight
    Height (tbd)
    BMI (tbd)
    pathType (tbd) - assume concrete for now
    distance
    Calories
    Age (tbd)
    const CalorieBurnRate = 0.0175
    */
    double heightInches = 0.0;
    double weightPounds = 0.0;
    string choice = "";
    double distance = 0.0;
    double calories = 0.0;

    getUserInput(heightInches, weightPounds, choice, distance, calories);
    if (choice == "1" || choice == "calories") {
        // Calculate calories burned based on weight and distance
        calculateCalories(weightPounds, distance); // Example distance of 5 km
    } else if (choice == "2" || choice == "distance") {
        // Calculate distance based on weight and calories burned
        calculateDistance(weightPounds, calories); // Example calories burned of 100
    } else {
        cout << "Something went wrong" << endl;
    }

    return 0;

}

void getUserInput(double& heightInches, double& weightPounds, string& choice, double& distance, double& calories) {
    do {
        cout << "Enter your height in inches: ";
        cin >> heightInches;
    } while (heightInches <= 0);
    do {
        cout << "Enter your weight in pounds: ";
        cin >> weightPounds;
    } while (weightPounds <= 0);
    do {
        cout << "Do you want to calculate \n" <<
            "1. [calories] burned by distance or\n" <<
            "2. [distance] for a certain distance?\n" << 
            "(Enter \'1\' or \'calories\' or \'2\' or \'distance\'): ";
        cin >> choice;
        if (choice != "1" && choice != "calories" && choice != "2" && choice != "distance") {
            cout << "Invalid choice. Please enter \'1\' " <<
            "or \'calories\' to calculate the calories burned " <<
            "or \'2\' or \'distance\' to calculate the distance needed." << 
            endl;
        }
    } while (choice != "1" && choice != "calories" && choice != "2" && choice != "distance");
    if (choice == "calories") {
        choice = "1";
    } else if (choice == "distance") {
        choice = "2";
    }
    if (choice == "1") {
        cout << "You chose to calculate calories burned by distance." << endl;
        do {
            cout << "Enter the distance in miles: ";
            cin >> distance;
        } while (distance <= 0);
    } else if (choice == "2") {
        cout << "You chose to calculate distance for a certain distance." << endl;
        do {
            cout << "Enter the calories burned: ";
            cin >> calories;
        } while (calories <= 0);
    }
    
}

void calculateCalories(double weight, double distance) {
    const double CalorieBurnRate = 0.0175;
    // Assuming distance is in kilometers and weight is in kilograms
    double caloriesBurned = CalorieBurnRate * weight * distance;
    std::cout << "Calories burned: " << caloriesBurned << std::endl;
}

void calculateDistance(double weight, double calories) {
    const double CalorieBurnRate = 0.0175;
    // Assuming weight is in kilograms and calories is the total calories burned
    double distance = calories / (CalorieBurnRate * weight);
    std::cout << "Distance needed: " << distance << " miles" << std::endl;
}