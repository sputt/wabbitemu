#include "stdafx.h"

#include "guisize.h"
#include "guiskin.h"
#include "guicutout.h"

extern BOOL silent_mode;

LRESULT HandleSizeMessage(HWND hwnd, LPCALC lpCalc) {
	u_int width;
	HMENU hMenu = GetMenu(hwnd);
	RECT rc, clientRect;
	CopyRect(&rc, &lpCalc->rectLCD);

	int scale = lpCalc->bSkinEnabled ? 2 : lpCalc->scale;
	int silentMode = silent_mode ? SWP_HIDEWINDOW : 0;
	int lcdWidth = (rc.right - rc.left) / DEFAULT_KEYMAP_SCALE * scale;
	int lcdHeight = (rc.bottom - rc.top) / DEFAULT_KEYMAP_SCALE * scale;

	GetClientRect(hwnd, &clientRect);
	width = clientRect.right - clientRect.left;

	if (!lpCalc->bSkinEnabled) {
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

	SetWindowPos(lpCalc->hwndLCD, NULL, rc.left, rc.top, lcdWidth, lcdHeight, SWP_NOZORDER | silentMode);

	// force little buttons to be correct
	if (lpCalc->bCutout && lpCalc->bSkinEnabled) {
		PositionLittleButtons(hwnd);
	}

	if (lpCalc->hwndStatusBar != NULL) {
		SendMessage(lpCalc->hwndStatusBar, WM_SIZE, SIZE_RESTORED, 0);
	}

	UpdateWindow(lpCalc->hwndLCD);
	return 0;
}

// TODO: better names and types
LRESULT HandleSizingMessage(HWND hwnd, LPCALC lpCalc, WPARAM wParam, RECT *prc) {
	if (lpCalc->bSkinEnabled) {
		return 1;
	}

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
		prc->left -= 128 / 4;
		break;
	default:
		prc->right += 128 / 4;
		break;
	}

	switch (wParam) {
	case WMSZ_TOPLEFT:
	case WMSZ_TOP:
	case WMSZ_TOPRIGHT:
		prc->top -= 64 / 4;
		break;
	default:
		prc->bottom += 64 / 4;
		break;
	}


	// Make sure the width is a nice clean proportional sizing
	AdjustWidth = (prc->right - prc->left - ClientAdjustWidth) % 128;
	AdjustHeight = (prc->bottom - prc->top - ClientAdjustHeight) % 64;

	int cx_mult = (prc->right - prc->left - ClientAdjustWidth) / 128;
	int cy_mult = (prc->bottom - prc->top - ClientAdjustHeight) / 64;

	while (cx_mult < 2 || cy_mult < 2) {
		if (cx_mult < 2) {cx_mult++; AdjustWidth -= 128;}
		if (cy_mult < 2) {cy_mult++; AdjustHeight -= 64;}
	}

	if (cx_mult > cy_mult) {
		AdjustWidth += (cx_mult - cy_mult) * 128;
	} else if (cy_mult > cx_mult) {
		AdjustHeight += (cy_mult - cx_mult) * 64;
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