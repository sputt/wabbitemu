#pragma once

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

BOOL SeekFileLabel(FILE *file, const TCHAR *lpszLabel);
BOOL Edit_Printf(HWND hwndEdit, TCHAR *szFormat, ...);
BOOL OnContextMenu(HWND hwnd, int x, int y, HMENU hmenu);
int ComboBox_SelectByItemData(HWND hwndCombo, LONG_PTR dwItemData);
LONG_PTR ComboBox_GetSelectionItemData(HWND hwndCombo);
HCURSOR GetCursorFromEdge(int Edge);
double round(double num);
void ShowLastError(PTSTR lpszFunction);