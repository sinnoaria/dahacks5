#pragma once

namespace fitnessGuiConfig {
struct TableColumn {
    const wchar_t* title;
    int width;
};

constexpr int heightId = 1001;
constexpr int weightId = 1002;
constexpr int distanceId = 1003;
constexpr int caloriesId = 1004;
constexpr int addResultId = 3001;
constexpr int saveResultsId = 3002;
constexpr int statusId = 3003;
constexpr int resultsTableId = 3004;

constexpr int windowWidth = 600;
constexpr int windowHeight = 650;
constexpr int inputX = 180;
constexpr int inputWidth = 180;
constexpr int labelX = 20;
constexpr int labelWidth = 150;
constexpr int inputHeight = 25;
constexpr int rowSpacing = 40;
constexpr int resultsTableX = 20;
constexpr int resultsTableY = 380;
constexpr int resultsTableWidth = 540;
constexpr int resultsTableHeight = 220;

constexpr TableColumn tableColumns[] = {
    {L"Height (in)", 90},
    {L"Weight (lb)", 90},
    {L"BMI", 80},
    {L"Distance (mi)", 100},
    {L"Calories", 100},
    {L"Timestamp", 230}
};
}
