#ifndef GUISIZE_H
#define GUISIZE_H

#include "gui.h"
#include "calc.h"

unsigned int GetDefaultKeymapScale(int model);
double GetSkinScale(LONG ClientNewWidth, LONG ClientNewHeight,
	LONG *DiffWidth, LONG *DiffHeight, double default_skin_scale);
LRESULT HandleMoveMessage(HWND hwnd, LPMAINWINDOW lpMainWindow);
LRESULT HandleSizeMessage(HWND hwnd, HWND hwndLcd, LPMAINWINDOW lpMainWindow, LPCALC lpCalc,
	BOOL isSkinEnabled, BOOL isCutout, RECT *newClientRect);
LRESULT HandleLCDSizingMessage(HWND hwnd, HWND hwndStatusBar, LPMAINWINDOW lpMainWindow, WPARAM wParam, RECT *prc, LONG lcdWidth);
LRESULT HandleSkinSizingMessage(HWND hwnd, LPMAINWINDOW lpMainWindow, WPARAM wParam, RECT *prc);
LRESULT GetMinMaxInfo(HWND hwnd, LPMAINWINDOW lpMainWindow, MINMAXINFO *info);

#endif