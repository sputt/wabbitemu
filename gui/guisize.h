#ifndef GUISIZE_H
#define GUISIZE_H

#include "calc.h"

unsigned int GetDefaultKeymapScale(int model);
LRESULT HandleMoveMessage(HWND hwnd, LPCALC lpCalc);
LRESULT HandleSizeMessage(HWND hwnd, HWND hwndLcd, LPCALC lpCalc, BOOL skinEnabled);
LRESULT HandleLCDSizingMessage(HWND hwnd, LPCALC lpCalc, WPARAM wParam, RECT *prc);
LRESULT HandleSkinSizingMessage(HWND hwnd, LPCALC lpCalc, WPARAM wParam, RECT *prc);
LRESULT GetMinMaxInfo(HWND hwnd, LPCALC lpCalc, MINMAXINFO *info);

#endif