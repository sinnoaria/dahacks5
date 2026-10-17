#pragma once

#ifndef UNICODE
#define UNICODE
#endif

#ifndef _UNICODE
#define _UNICODE
#endif

#include <windows.h>
#include <commctrl.h>

#include <fstream>
#include <string>
#include <vector>

namespace {

struct FileResult {
    std::string height;
    std::string weight;
    std::string bmi;
    std::string distance;
    std::string calories;
    std::string timestamp;
};

std::string fieldValue(const std::string& line, const char* label) {
    const std::string prefix = std::string(label) + ": ";
    if (line.compare(0, prefix.size(), prefix) != 0) {
        return {};
    }
    return line.substr(prefix.size());
}

std::vector<FileResult> readResults(
    const char* fileName,
    bool& hasInvalidRecord
) {
    hasInvalidRecord = false;
    std::ifstream input(fileName);
    std::vector<FileResult> results;
    if (!input) {
        return results;
    }

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(input, line)) {
        if (!line.empty()) {
            lines.push_back(line);
        }
    }

    constexpr size_t fieldsPerResult = 6;
    for (size_t index = 0; index + fieldsPerResult <= lines.size();
         index += fieldsPerResult) {
        FileResult result{
            fieldValue(lines[index], "Height"),
            fieldValue(lines[index + 1], "Weight"),
            fieldValue(lines[index + 2], "BMI"),
            fieldValue(lines[index + 3], "Distance"),
            fieldValue(lines[index + 4], "Calories Burned"),
            fieldValue(lines[index + 5], "Timestamp")
        };

        if (result.height.empty() || result.weight.empty() ||
            result.bmi.empty() || result.distance.empty() ||
            result.calories.empty() || result.timestamp.empty()) {
            hasInvalidRecord = true;
            continue;
        }
        results.push_back(result);
    }

    hasInvalidRecord = hasInvalidRecord ||
        lines.size() % fieldsPerResult != 0;
    return results;
}

std::wstring toWide(const std::string& value) {
    return std::wstring(value.begin(), value.end());
}

void addColumn(HWND table, int index, const wchar_t* title, int width) {
    LVCOLUMNW column = {};
    column.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
    column.pszText = const_cast<wchar_t*>(title);
    column.cx = width;
    column.iSubItem = index;
    ListView_InsertColumn(table, index, &column);
}

}

HWND createFitnessResultsTable(
    HWND parent,
    int x,
    int y,
    int width,
    int height
) {
    HWND table = CreateWindowExW(
        WS_EX_CLIENTEDGE,
        WC_LISTVIEWW,
        L"",
        WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL |
            LVS_SHOWSELALWAYS | WS_VSCROLL | WS_HSCROLL,
        x,
        y,
        width,
        height,
        parent,
        nullptr,
        GetModuleHandleW(nullptr),
        nullptr
    );

    if (table == nullptr) {
        return nullptr;
    }

    ListView_SetExtendedListViewStyle(
        table,
        LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER
    );
    addColumn(table, 0, L"Height (in)", 90);
    addColumn(table, 1, L"Weight (lb)", 90);
    addColumn(table, 2, L"BMI", 80);
    addColumn(table, 3, L"Distance (mi)", 100);
    addColumn(table, 4, L"Calories", 100);
    addColumn(table, 5, L"Timestamp", 230);
    return table;
}

bool refreshFitnessResultsTable(HWND table, const char* fileName) {
    if (table == nullptr || fileName == nullptr) {
        return false;
    }

    bool hasInvalidRecord = false;
    const std::vector<FileResult> results =
        readResults(fileName, hasInvalidRecord);
    ListView_DeleteAllItems(table);

    for (size_t index = 0; index < results.size(); ++index) {
        const FileResult& result = results[index];
        const std::wstring values[] = {
            toWide(result.height),
            toWide(result.weight),
            toWide(result.bmi),
            toWide(result.distance),
            toWide(result.calories),
            toWide(result.timestamp)
        };

        LVITEMW item = {};
        item.mask = LVIF_TEXT;
        item.iItem = static_cast<int>(index);
        item.pszText = const_cast<wchar_t*>(values[0].c_str());
        ListView_InsertItem(table, &item);
        for (int column = 1; column < 6; ++column) {
            ListView_SetItemText(
                table,
                static_cast<int>(index),
                column,
                const_cast<wchar_t*>(values[column].c_str())
            );
        }
    }

    return !hasInvalidRecord;
}
