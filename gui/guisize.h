#ifndef GUISIZE_H
#define GUISIZE_H

#include "calc.h"

LRESULT HandleSizeMessage(HWND hwnd, LPCALC lpCalc);
LRESULT HandleSizingMessage(HWND hwnd, LPCALC lpCalc, WPARAM wParam, RECT *prc);

#endif