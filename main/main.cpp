
#include <iostream>
#include <string>
using namespace std; //include all std for simplicity

void getUserInput(
    double& heightInches,
    double& weightPounds,
    int& choice,
    double& distance,
    double& calories
);

void calculateCalories(double weight, double distance);
void calculateDistance(double weight, double calories);
void showBMI(double heightInches, double weightPounds);
double getUserHeight();
double getUserWeight();
double getUserDistance();
double getUserCalories();
int getChoice();
int getPathType();
int getMovementType();
void clearCin(double& value, double defaultFail);
void clearCin(int& value, int defaultFail);
void clearCin();


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

    getUserInput(heightInches, weightPounds, choice, distance, calories);
    if (choice == 1) {
        // Calculate calories burned based on weight and distance
        calculateCalories(weightPounds, distance); // Example distance of 5 mi
    } else if (choice == 2) {
        // Calculate distance based on weight and calories burned
        calculateDistance(weightPounds, calories); // Example calories burned of 100
    } else {
        cout << "Something went wrong" << endl;
    }
    showBMI(heightInches, weightPounds);

    return 0;

}

void clearCin(double& value, double defaultFail) {
    if (cin.fail()) value = defaultFail; // Reset the value to a default fail value
    cin.clear(); // Clear any error flags
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear the input buffera
}

void clearCin(int& value, int defaultFail) {
    if (cin.fail()) value = defaultFail; // Reset the value to a default fail value
    cin.clear(); // Clear any error flags
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear the input buffer
}

void clearCin() {
    cin.clear(); // Clear any error flags
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear the input buffer
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

void calculateCalories(double weight, double distance) {
    const double CalorieBurnRate = 0.76;
    // Assuming distance is in kilometers and weight is in kilograms
    double caloriesBurned = CalorieBurnRate * weight * distance;
    std::cout << "Calories burned: " << caloriesBurned << endl;
}

void calculateDistance(double weight, double calories) {
    const double CalorieBurnRate = 0.76;
    // Assuming weight is in kilograms and calories is the total calories burned
    double distance = calories / (CalorieBurnRate * weight);
    std::cout << "Distance needed: " << distance << " miles" << endl;
}

void showBMI(double heightInches, double weightPounds) {
    // BMI = (weight in pounds / (height in inches * height in inches)) * 703
    double bmi = (weightPounds / (heightInches * heightInches)) * 703;
    std::cout << "Your BMI is: " << bmi << endl;
}