#ifndef GUIBUTTONS_H
#define GUIBUTTONS_H

#include "gui.h"
#include "calc.h"

typedef enum {
	DBS_UP = 0x00,
	DBS_DOWN = 0x01,
	DBS_LOCK = 0x02,
	DBS_PRESS = 0x04,
	DBS_COPY = 0x08,
} DisplayButtonState;

void FindButtonsRect(BitmapData *data);
void FinalizeButtons(LPMAINWINDOW lpMainWindow, LPCALC lpCalc);
void HandleKeyDown(LPMAINWINDOW lpMainWindow, LPCALC lpCalc, WPARAM);
void HandleKeyUp(LPMAINWINDOW lpMainWindow, LPCALC lpCalc, WPARAM);
void DrawButtonState(HDC, HDC, RECT *, UINT state, UINT keymap_scale);
void DrawButtonStateNoSkin(HDC hdcDest, HDC hdcSkin, HDC hdcKeymap, RECT brect, UINT state, UINT keymap_scale);
void DrawButtonStatesAll(LPCALC lpCalc, HDC hdcSkin, HDC hdcKeymap);
void LogKeypress(LPCALC lpCalc, int group, int bit);
HBITMAP DrawButtonAndMask(LPCALC lpCalc, POINT pt, HBITMAP *hbmButton, HBITMAP *hbmMask);
void DrawButtonShadow(HDC hdc, HDC hdcKeymap, RECT brect, UINT keymap_scale);

#endif /* GUIBUTTONS_H */
