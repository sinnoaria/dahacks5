#include "fitnessGui.h"
#include "FitnessResults.h"
#include "fitnessGuiConfig.h"

#include <windows.h>

#include <cmath>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>

#pragma comment(lib, "user32.lib")

HWND g_heightEdit = nullptr;
HWND g_weightEdit = nullptr;
HWND g_distanceEdit = nullptr;
HWND g_caloriesEdit = nullptr;
HWND g_bmiResult = nullptr;
HWND g_caloriesResult = nullptr;
HWND g_distanceResult = nullptr;
HWND g_status = nullptr;
std::vector<FitnessResults>* g_results = nullptr;

HWND createControl(
    const wchar_t* className,
    const wchar_t* text,
    DWORD style,
    int x,
    int y,
    int width,
    int height,
    HWND parent,
    int id
) {
    return CreateWindowW(
        className,
        text,
        WS_CHILD | WS_VISIBLE | style,
        x,
        y,
        width,
        height,
        parent,
        reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)),
        GetModuleHandleW(nullptr),
        nullptr
    );
}

bool readDouble(HWND control, double& value) {
    wchar_t buffer[128] = {};
    GetWindowTextW(control, buffer, ARRAYSIZE(buffer));
    if (buffer[0] == L'\0') {
        return false;
    }

    wchar_t* end = nullptr;
    value = wcstod(buffer, &end);
    return end != buffer && *end == L'\0' &&
        std::isfinite(value) && value > 0.0;
}

void setText(HWND control, const std::wstring& text) {
    SetWindowTextW(control, text.c_str());
}

void updateResults() {
    double height = 0.0;
    double weight = 0.0;
    double distance = 0.0;
    double calories = 0.0;
    const bool hasHeight = readDouble(g_heightEdit, height);
    const bool hasWeight = readDouble(g_weightEdit, weight);
    const bool hasDistance = readDouble(g_distanceEdit, distance);
    const bool hasCalories = readDouble(g_caloriesEdit, calories);

    FitnessResults currentResult;
    currentResult.setHeightInches(height);
    currentResult.setWeightPounds(weight);

    if (hasHeight && hasWeight) {
        currentResult.setCalculatedBMI();
        std::wostringstream output;
        output << L"BMI: " << std::fixed << std::setprecision(2)
            << currentResult.getBMI();
        setText(g_bmiResult, output.str());
    } else {
        setText(g_bmiResult, L"BMI: enter height and weight");
    }

    if (hasWeight && hasDistance) {
        currentResult.setDistanceMiles(distance);
        currentResult.calculateCaloriesBurned();
        std::wostringstream output;
        output << L"Calories for distance: " << std::fixed
            << std::setprecision(2) << currentResult.getCaloriesBurned();
        setText(g_caloriesResult, output.str());
    } else {
        setText(g_caloriesResult, L"Calories for distance: enter weight and distance");
    }

    if (hasWeight && hasCalories) {
        currentResult.setCaloriesBurned(calories);
        currentResult.calculateDistanceForCalories();
        std::wostringstream output;
        output << L"Distance for calories: " << std::fixed
            << std::setprecision(2) << currentResult.getDistanceMiles()
            << L" miles";
        setText(g_distanceResult, output.str());
    } else {
        setText(g_distanceResult, L"Distance for calories: enter weight and calories");
    }
}

bool readValues(double& height, double& weight, double& distance, double& calories) {
    const bool hasHeight = readDouble(g_heightEdit, height);
    const bool hasWeight = readDouble(g_weightEdit, weight);
    const bool hasDistance = readDouble(g_distanceEdit, distance);
    const bool hasCalories = readDouble(g_caloriesEdit, calories);

    if (!hasHeight || !hasWeight || (!hasDistance && !hasCalories)) {
        return false;
    }

    FitnessResults result;
    result.setHeightInches(height);
    result.setWeightPounds(weight);

    if (hasDistance) {
        result.setDistanceMiles(distance);
        result.calculateCaloriesBurned();
        calories = result.getCaloriesBurned();
    } else {
        result.setCaloriesBurned(calories);
        result.calculateDistanceForCalories();
        distance = result.getDistanceMiles();
    }
    return true;
}

bool createCurrentResult(FitnessResults& result) {
    double height = 0.0;
    double weight = 0.0;
    double distance = 0.0;
    double calories = 0.0;
    if (!readValues(height, weight, distance, calories)) {
        return false;
    }

    result.setHeightInches(height);
    result.setWeightPounds(weight);
    result.setDistanceMiles(distance);
    result.setCaloriesBurned(calories);
    result.setCalculatedBMI();
    result.setTimestamp(std::time(nullptr));
    return true;
}

void addCurrentResult() {
    FitnessResults result;
    if (!createCurrentResult(result)) {
        setText(g_status, L"Enter height, weight, and distance or calories.");
        return;
    }

    g_results->push_back(result);
    setText(g_status, L"Result added to the results list.");
}

void saveResults() {
    if (g_results->empty()) {
        setText(g_status, L"Add a result before saving.");
        return;
    }

    std::ofstream output("fitness_results.txt", std::ios::app);
    if (!output) {
        setText(g_status, L"Unable to open fitness_results.txt.");
        return;
    }

    FitnessResults result = g_results->back();
        output << std::fixed << std::setprecision(2)
            << "Height: " << result.getHeightInches() << " inches\n"
            << "Weight: " << result.getWeightPounds() << " pounds\n"
            << "BMI: " << result.getBMI() << "\n"
            << "Distance: " << result.getDistanceMiles() << " miles\n"
            << "Calories Burned: " << result.getCaloriesBurned() << "\n"
            << "Timestamp: " << result.getTimestamp() << "\n\n";
    

    setText(g_status, output ? L"Results saved." : L"Unable to write results.");
}

void createWindowControls(HWND window) {
    createControl(
        L"STATIC", L"Height (inches):", 0,
        fitnessGuiConfig::labelX, 20, fitnessGuiConfig::labelWidth,
        fitnessGuiConfig::inputHeight, window, 0
    );
    g_heightEdit = createControl(
        L"EDIT", L"", WS_BORDER | ES_AUTOHSCROLL,
        fitnessGuiConfig::inputX, 18, fitnessGuiConfig::inputWidth,
        fitnessGuiConfig::inputHeight, window, fitnessGuiConfig::heightId
    );
    createControl(
        L"STATIC", L"Weight (pounds):", 0,
        fitnessGuiConfig::labelX, 60, fitnessGuiConfig::labelWidth,
        fitnessGuiConfig::inputHeight, window, 0
    );
    g_weightEdit = createControl(
        L"EDIT", L"", WS_BORDER | ES_AUTOHSCROLL,
        fitnessGuiConfig::inputX, 58, fitnessGuiConfig::inputWidth,
        fitnessGuiConfig::inputHeight, window, fitnessGuiConfig::weightId
    );
    createControl(
        L"STATIC", L"Distance (miles):", 0,
        fitnessGuiConfig::labelX, 100, fitnessGuiConfig::labelWidth,
        fitnessGuiConfig::inputHeight, window, 0
    );
    g_distanceEdit = createControl(
        L"EDIT", L"", WS_BORDER | ES_AUTOHSCROLL,
        fitnessGuiConfig::inputX, 98, fitnessGuiConfig::inputWidth,
        fitnessGuiConfig::inputHeight, window, fitnessGuiConfig::distanceId
    );
    createControl(
        L"STATIC", L"Calories burned:", 0,
        fitnessGuiConfig::labelX, 140, fitnessGuiConfig::labelWidth,
        fitnessGuiConfig::inputHeight, window, 0
    );
    g_caloriesEdit = createControl(
        L"EDIT", L"", WS_BORDER | ES_AUTOHSCROLL,
        fitnessGuiConfig::inputX, 138, fitnessGuiConfig::inputWidth,
        fitnessGuiConfig::inputHeight, window, fitnessGuiConfig::caloriesId
    );
    g_bmiResult = createControl(
        L"STATIC", L"BMI: enter height and weight",
        0, 20, 210, 500, 25, window, 0
    );
    g_caloriesResult = createControl(
        L"STATIC", L"Calories for distance: enter weight and distance",
        0, 20, 250, 500, 25, window, 0
    );
    g_distanceResult = createControl(
        L"STATIC", L"Distance for calories: enter weight and calories",
        0, 20, 290, 500, 25, window, 0
    );
    createControl(
        L"BUTTON", L"Add to Results", BS_PUSHBUTTON,
        20, 330, 140, 30, window, fitnessGuiConfig::addResultId
    );
    createControl(
        L"BUTTON", L"Save Results", BS_PUSHBUTTON,
        180, 330, 140, 30, window, fitnessGuiConfig::saveResultsId
    );
    g_status = createControl(
        L"STATIC", L"Results are not saved automatically.",
        0, 340, 370, 220, 30, window, fitnessGuiConfig::statusId
    );
}

LRESULT CALLBACK windowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE:
        createWindowControls(window);
        return 0;
    case WM_COMMAND:
        if (HIWORD(wParam) == BN_CLICKED) {
            if (LOWORD(wParam) == fitnessGuiConfig::addResultId) {
                addCurrentResult();
                return 0;
            }
            if (LOWORD(wParam) == fitnessGuiConfig::saveResultsId) {
                saveResults();
                return 0;
            }
        }
        if (HIWORD(wParam) == EN_CHANGE) {
            switch (LOWORD(wParam)) {
            case fitnessGuiConfig::heightId:
            case fitnessGuiConfig::weightId:
            case fitnessGuiConfig::distanceId:
            case fitnessGuiConfig::caloriesId:
                updateResults();
                return 0;
            default:
                break;
            }
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        break;
    }
    return DefWindowProcW(window, message, wParam, lParam);
}

int runFitnessGui(std::vector<FitnessResults>& results) {
    g_results = &results;
    HINSTANCE instance = GetModuleHandleW(nullptr);
    constexpr wchar_t windowClassName[] = L"FitnessCalculatorWindow";
    WNDCLASSW windowClass = {};
    windowClass.lpfnWndProc = windowProc;
    windowClass.hInstance = instance;
    windowClass.lpszClassName = windowClassName;
    windowClass.hCursor = LoadCursorW(nullptr, MAKEINTRESOURCEW(32512));

    if (!RegisterClassW(&windowClass)) {
        return 1;
    }

    HWND window = CreateWindowExW(
        0, windowClassName, L"Fitness Calculator", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        fitnessGuiConfig::windowWidth, fitnessGuiConfig::windowHeight,
        nullptr, nullptr, instance, nullptr
    );
    if (window == nullptr) {
        return 1;
    }

    ShowWindow(window, SW_SHOW);
    UpdateWindow(window);

    MSG message = {};
    while (GetMessageW(&message, nullptr, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }
    return static_cast<int>(message.wParam);
}
