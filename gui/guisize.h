#ifndef GUISIZE_H
#define GUISIZE_H

#include "calc.h"

int GetDefaultKeymapScale(int model);
LRESULT HandleSizeMessage(HWND hwnd, HWND hwndLcd, LPCALC lpCalc, BOOL skinEnabled);
LRESULT HandleSizingMessage(HWND hwnd, LPCALC lpCalc, WPARAM wParam, RECT *prc);

#endif