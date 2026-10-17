#include <iostream>
#include <string>
#include <time.h>
using namespace std;


class FitnessResults {
    private:
        double heightInches;
        double weightPounds;
        double bmi;
        double distanceMiles;
        double caloriesBurned;
        int pathType;
        int movementType;
        int age;
        string timestamp;
    
    public:
    
        double getHeightInches() {
            return heightInches;
        }

        double getWeightPounds() {
            return weightPounds;
        }

        double getBMI() {
            return bmi;
        }

        double getDistanceMiles() {
            return distanceMiles;
        }

        double getCaloriesBurned() {
            return caloriesBurned;
        }

        string getTimestamp() {
            return timestamp;
        }

        string getPathType() {
            return pathType == 1 ? "Concrete" : (pathType == 2 ? 
                "Dirt" : "Unknown");
        }

        string getMovementType() {
            return movementType == 1 ? "Walking" : (movementType == 2 ? 
                "Running" : "Unknown");
        }

        int getAge() {
            return age;
        }

        void setHeightInches(double height) {
            heightInches = height;
        }

        void setWeightPounds(double weight) {
            weightPounds = weight;
        }

        void setDistanceMiles(double distance) {
            distanceMiles = distance;
        }

        void setCaloriesBurned(double calories) {
            caloriesBurned = calories;
        }

        void setPathType(int type) {
            pathType = type;
        }

        void setMovementType(int type) {
            movementType = type;
        }

        void setAge(int userAge) {
            age = userAge;
        }

        void setTimestamp(time_t time) {
            timestamp = ctime(&time);
        }

        void setBMI(double bmiValue) {
            bmi = bmiValue;
        }

        void setCalculatedBMI() {
            bmi = calculateBMI();
        }

        double calculateBMI() {
            return (weightPounds / (heightInches * heightInches)) * 703.0;
        }

        void calculateCaloriesBurned() {
            const double CalorieBurnRate = 0.76;
            caloriesBurned = CalorieBurnRate * weightPounds * distanceMiles;
        }

        void calculateDistanceForCalories() {
            const double CalorieBurnRate = 0.76;
            distanceMiles = caloriesBurned / (CalorieBurnRate * weightPounds);
        }

        void displayResults() const {
            cout << "Height: " << heightInches << " inches" << endl;
            cout << "Weight: " << weightPounds << " pounds" << endl;
            cout << "BMI: " << bmi << endl;
            cout << "Distance: " << distanceMiles << " miles" << endl;
            cout << "Calories Burned: " << caloriesBurned << endl;
            cout << "Timestamp: " << timestamp << endl;
        }
};