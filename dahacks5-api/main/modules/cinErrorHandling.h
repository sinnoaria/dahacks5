#pragma once

#include <iostream>
#include <limits>
using std::cin;
using std::numeric_limits;
using std::streamsize;

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