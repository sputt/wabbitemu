#include "stdafx.h"

#include "guisize.h"
#include "guicutout.h"

extern BOOL silent_mode;

LRESULT HandleSizeMessage(HWND hwnd, LPCALC lpCalc) {
	RECT rc, screen;
	GetClientRect(hwnd, &rc);
	HMENU hmenu = GetMenu(hwnd);
	int cyMenu = hmenu == NULL ? 0 : GetSystemMetrics(SM_CYMENU);
	if (lpCalc->bCutout && lpCalc->SkinEnabled) {
		rc.bottom += cyMenu;
	}

	int desired_height = lpCalc->SkinEnabled ?  lpCalc->rectSkin.bottom : 128;

	int status_height;
	if (lpCalc->hwndStatusBar == NULL) {
		status_height = 0;
	} else {
		RECT src;
		GetWindowRect(lpCalc->hwndStatusBar, &src);

		status_height = src.bottom - src.top;
		desired_height += status_height;
	}

	rc.bottom -= status_height;

	float xc = 1.0, yc = 1.0;
	if (!lpCalc->SkinEnabled) {
		xc = ((float) rc.right) / 256.0f;
		yc = ((float) rc.bottom) / 128.0f;
	} else {
		int screenWidth = 1200;//GetSystemMetrics(SM_CXVIRTUALSCREEN);
		int screenHeight = 600;//GetSystemMetrics(SM_CYVIRTUALSCREEN);

		// were too big to fit on the screen
		if (rc.bottom > screenHeight || rc.right > screenWidth) {
			float scale = min((float) rc.bottom / screenHeight, (float) rc.right / screenWidth);
			rc.bottom *= .9;//scale;
			rc.right *= .9;//scale;
		}
	}

	int width = lpCalc->rectLCD.right - lpCalc->rectLCD.left;
	SetRect(&screen,
		0, 0,
		(int) (width * xc),
		(int) (64 * 2 * yc));

	if (lpCalc->SkinEnabled) {
		OffsetRect(&screen, lpCalc->rectLCD.left, lpCalc->rectLCD.top);
	} else {
		OffsetRect(&screen, (int) ((rc.right - width * xc) / 2), 0);
	}

	HWND hwndAfter = lpCalc->bAlwaysOnTop ? HWND_TOPMOST : HWND_NOTOPMOST;
	UINT silentMode = silent_mode ? SWP_HIDEWINDOW : 0;

	RECT client;
	client.top = 0;
	client.left = 0;
	if (lpCalc->SkinEnabled) {
		if (lpCalc->bCutout) {
			GetWindowRect(hwnd, &client);
		}

		RECT correctSize = lpCalc->rectSkin;
		AdjustWindowRect(&correctSize, (WS_TILEDWINDOW |  WS_VISIBLE | WS_CLIPCHILDREN) & ~(WS_MAXIMIZEBOX), cyMenu);
		if (correctSize.left < 0) {
			correctSize.right -= correctSize.left;
		}

		if (correctSize.top < 0) {
			correctSize.bottom -= correctSize.top;
		}

		SetWindowPos(hwnd, hwndAfter, 0, 0, correctSize.right, correctSize.bottom, SWP_NOACTIVATE | 
			SWP_NOMOVE | SWP_DRAWFRAME | silentMode);
	} else {
		SetWindowPos(lpCalc->hwndFrame, hwndAfter, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | silentMode);
	}

	RECT windowRect;
	GetWindowRect(hwnd, &windowRect);

	if (windowRect.bottom - windowRect.top != screen.bottom - screen.top ||
		windowRect.right - windowRect.left != screen.right - screen.left)
	{
		MoveWindow(lpCalc->hwndLCD, screen.left + client.left, screen.top + client.top,
			screen.right - screen.left, screen.bottom - screen.top, FALSE);
	}

	ValidateRect(hwnd, &screen);
	if (lpCalc->hwndStatusBar != NULL) {
		SendMessage(lpCalc->hwndStatusBar, WM_SIZE, 0, 0);
	}

	//force little buttons to be correct
	PositionLittleButtons(hwnd);
	UpdateWindow(lpCalc->hwndLCD);
	return 0;
}

// TODO: better names and types
LRESULT HandleSizingMessage(HWND hwnd, LPCALC lpCalc, WPARAM wParam, RECT *prc) {
	if (lpCalc->SkinEnabled) {
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