#include "stdafx.h"

#include "guisize.h"
#include "guiskin.h"
#include "guicutout.h"

extern BOOL silent_mode;

int GetDefaultKeymapScale(int model) {
	switch (model) {
	case TI_84PCSE:
		return DEFAULT_84PCSE_KEYMAP_SCALE;
	default:
		return DEFAULT_KEYMAP_SCALE;
	}
}

LRESULT HandleSizeMessage(HWND hwnd, HWND hwndLcd, LPCALC lpCalc, BOOL skinEnabled) {
	u_int width;
	HMENU hMenu = GetMenu(hwnd);
	RECT rc, clientRect;
	CopyRect(&rc, &lpCalc->rectLCD);

	int default_scale = GetDefaultKeymapScale(lpCalc->model);
	int scale = skinEnabled ? default_scale : lpCalc->scale;
	int silentMode = silent_mode ? SWP_HIDEWINDOW : 0;
	int lcdWidth = skinEnabled ? (rc.right - rc.left) / default_scale : lpCalc->cpu.pio.lcd->display_width;
	lcdWidth *= scale;
	int lcdHeight = skinEnabled ? (rc.bottom - rc.top) / default_scale : lpCalc->cpu.pio.lcd->height;
	lcdHeight *= scale;

	GetClientRect(hwnd, &clientRect);
	width = clientRect.right - clientRect.left;

	if (!skinEnabled) {
		rc.top = 0;
		rc.left = 0;
		if (width > lcdWidth) {
			// if the lcd is less than client, center the lcd
			rc.left += (width - lcdWidth) / 2;
		}
	} else if (lpCalc->bCutout) {
		GetWindowRect(hwnd, &rc);
		OffsetRect(&rc, lpCalc->rectLCD.left, lpCalc->rectLCD.top);
	}

	SetWindowPos(hwndLcd, NULL, rc.left, rc.top, lcdWidth, lcdHeight, SWP_NOZORDER | silentMode);

	// force little buttons to be correct
	if (lpCalc->bCutout && skinEnabled) {
		PositionLittleButtons(hwnd);
	}

	if (lpCalc->hwndStatusBar != NULL) {
		SendMessage(lpCalc->hwndStatusBar, WM_SIZE, SIZE_RESTORED, 0);
	}

	//SetWindowPos(hwnd, NULL, 0, 0, clientWidth, clientHeight, SWP_NOMOVE | SWP_NOZORDER);

	UpdateWindow(hwndLcd);
	return 0;
}

// TODO: better names and types
LRESULT HandleSizingMessage(HWND hwnd, LPCALC lpCalc, WPARAM wParam, RECT *prc) {
	LCDBase_t *lcd = lpCalc->cpu.pio.lcd;
	LONG ClientAdjustWidth, ClientAdjustHeight;
	LONG AdjustWidth, AdjustHeight;

	// Adjust for border and menu
	RECT rc = {0, 0, 0, 0};
	AdjustWindowRect(&rc, WS_CAPTION | WS_TILEDWINDOW, FALSE);
	if (GetMenu(hwnd) != NULL) {
		rc.bottom += GetSystemMetrics(SM_CYMENU);
	}

	RECT src;
	if (lpCalc->hwndStatusBar != NULL) {
		GetWindowRect(lpCalc->hwndStatusBar, &src);
		rc.bottom += src.bottom - src.top;
	}
	//don't allow resizing from the sides
	if (wParam == WMSZ_LEFT || wParam == WMSZ_RIGHT 
		|| wParam == WMSZ_TOP || wParam == WMSZ_BOTTOM) {
			GetWindowRect(hwnd, &rc);
			memcpy(prc, &rc, sizeof(RECT));
			return 1;
	}

	ClientAdjustWidth = rc.right - rc.left;
	ClientAdjustHeight = rc.bottom - rc.top;


	switch (wParam) {
	case WMSZ_BOTTOMLEFT:
	case WMSZ_LEFT:
	case WMSZ_TOPLEFT:
		prc->left -= lcd->width / 4;
		break;
	default:
		prc->right += lcd->width / 4;
		break;
	}

	switch (wParam) {
	case WMSZ_TOPLEFT:
	case WMSZ_TOP:
	case WMSZ_TOPRIGHT:
		prc->top -= lcd->height/ 4;
		break;
	default:
		prc->bottom += lcd->height / 4;
		break;
	}


	// Make sure the width is a nice clean proportional sizing
	AdjustWidth = (prc->right - prc->left - ClientAdjustWidth) % lcd->width;
	AdjustHeight = (prc->bottom - prc->top - ClientAdjustHeight) % lcd->height;

	int cx_mult = (prc->right - prc->left - ClientAdjustWidth) / lcd->width;
	int cy_mult = (prc->bottom - prc->top - ClientAdjustHeight) / lcd->height;

	int min_scale = GetDefaultKeymapScale(lpCalc->model);
	while (cx_mult < min_scale || cy_mult < min_scale) {
		if (cx_mult < min_scale) { cx_mult++; AdjustWidth -= lcd->width; }
		if (cy_mult < min_scale) { cy_mult++; AdjustHeight -= lcd->height; }
	}

	if (cx_mult > cy_mult) {
		AdjustWidth += (cx_mult - cy_mult) * lcd->width;
	} else if (cy_mult > cx_mult) {
		AdjustHeight += (cy_mult - cx_mult) * lcd->height;
	}


	lpCalc->scale = min(cx_mult, cy_mult);

	switch (wParam) {
	case WMSZ_BOTTOMLEFT:
	case WMSZ_LEFT:
	case WMSZ_TOPLEFT:
		prc->left += AdjustWidth;
		break;
	default:
		prc->right -= AdjustWidth;
		break;
	}

	switch (wParam) {
	case WMSZ_TOPLEFT:
	case WMSZ_TOP:
	case WMSZ_TOPRIGHT:
		prc->top += AdjustHeight;
		break;
	default:
		prc->bottom -= AdjustHeight;
		break;
	}
	RECT rect;
	GetClientRect(hwnd, &rect);
	InvalidateRect(hwnd, &rect, TRUE);
	return 1;
}