#pragma once

#include <windows.h>

HWND createFitnessResultsTable(HWND parent, int x, int y, int width, int height);
bool refreshFitnessResultsTable(HWND table, const char* fileName);
