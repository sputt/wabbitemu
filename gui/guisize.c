#include "stdafx.h"

#include "guisize.h"
#include "gui.h"
#include "guiskin.h"
#include "guicutout.h"

extern BOOL silent_mode;

unsigned int GetDefaultKeymapScale(int model) {
	switch (model) {
	case TI_84PCSE:
		return DEFAULT_84PCSE_KEYMAP_SCALE;
	default:
		return DEFAULT_KEYMAP_SCALE;
	}
}

LRESULT HandleMoveMessage(HWND hwnd, LPCALC lpCalc) {
	if (lpCalc->bCutout && lpCalc->bSkinEnabled) {
		UINT left = (LONG)(lpCalc->rectLCD.left * lpCalc->skin_scale);
		UINT top = (LONG)(lpCalc->rectLCD.top * lpCalc->skin_scale);

		HDWP hdwp = BeginDeferWindowPos(3);
		RECT rc;
		GetWindowRect(hwnd, &rc);
		OffsetRect(&rc, left, top);
		DeferWindowPos(hdwp, lpCalc->hwndLCD, HWND_TOP, rc.left, rc.top, 0, 0, SWP_NOSIZE);
		EndDeferWindowPos(hdwp);
		PositionLittleButtons(hwnd);
	}

	return 0;
}

LRESULT HandleSizeMessage(HWND hwnd, HWND hwndLcd, LPCALC lpCalc, BOOL skinEnabled) {
	u_int width;
	HMENU hMenu = GetMenu(hwnd);
	RECT rc, clientRect;
	CopyRect(&rc, &lpCalc->rectLCD);

	UINT default_scale = GetDefaultKeymapScale(lpCalc->model);
	UINT scale = skinEnabled ? default_scale : max(lpCalc->scale, default_scale);
	int silentMode = silent_mode ? SWP_HIDEWINDOW : 0;
	UINT lcdWidth, lcdHeight;
	if (skinEnabled) {
		lcdWidth = (rc.right - rc.left) / default_scale;
		lcdHeight = (rc.bottom - rc.top) / default_scale;
		lcdWidth = (unsigned int)(lcdWidth * lpCalc->skin_scale);
		lcdHeight = (unsigned int)(lcdHeight *lpCalc->skin_scale);
	} else {
		lcdWidth = lpCalc->cpu.pio.lcd->display_width;
		lcdHeight = lpCalc->cpu.pio.lcd->height;
	}

	lcdWidth *= scale;
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
		UINT left = (LONG)(lpCalc->rectLCD.left * lpCalc->skin_scale);
		UINT top = (LONG)(lpCalc->rectLCD.top * lpCalc->skin_scale);

		GetWindowRect(hwnd, &rc);
		OffsetRect(&rc, left, top);
	} else {
		rc.left = (LONG)(rc.left * lpCalc->skin_scale);
		rc.top = (LONG)(rc.top * lpCalc->skin_scale);
	}

	MoveWindow(hwndLcd, rc.left, rc.top, lcdWidth, lcdHeight, TRUE);

	// force little buttons to be correct
	if (lpCalc->bCutout && skinEnabled) {
		PositionLittleButtons(hwnd);
	}

	if (lpCalc->hwndStatusBar != NULL) {
		SendMessage(lpCalc->hwndStatusBar, WM_SIZE, SIZE_RESTORED, 0);
	}

	UpdateWindow(hwndLcd);
	return 0;
}

LRESULT HandleLCDSizingMessage(HWND hwnd, HWND hwndStatusBar, LPCALC lpCalc, WPARAM wParam, RECT *prc, LONG lcdWidth) {
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
	if (hwndStatusBar != NULL) {
		GetWindowRect(hwndStatusBar, &src);
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
		prc->left -= lcdWidth / 4;
		break;
	default:
		prc->right += lcdWidth / 4;
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
	AdjustWidth = (prc->right - prc->left - ClientAdjustWidth) % lcdWidth;
	AdjustHeight = (prc->bottom - prc->top - ClientAdjustHeight) % lcd->height;

	int cx_mult = (prc->right - prc->left - ClientAdjustWidth) / lcdWidth;
	int cy_mult = (prc->bottom - prc->top - ClientAdjustHeight) / lcd->height;

	int min_scale = GetDefaultKeymapScale(lpCalc->model);
	while (cx_mult < min_scale || cy_mult < min_scale) {
		if (cx_mult < min_scale) { cx_mult++; AdjustWidth -= lcdWidth; }
		if (cy_mult < min_scale) { cy_mult++; AdjustHeight -= lcd->height; }
	}

	if (cx_mult > cy_mult) {
		AdjustWidth += (cx_mult - cy_mult) * lcdWidth;
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

BOOL UnadjustWindowRect(LPRECT prc, DWORD dwStyle, BOOL fMenu) {
	RECT rc;
	SetRectEmpty(&rc);
	BOOL fRc = AdjustWindowRect(&rc, dwStyle, fMenu);
	if (fRc) {
		prc->left -= rc.left;
		prc->top -= rc.top;
		prc->right -= rc.right;
		prc->bottom -= rc.bottom;
	}
	return fRc;
}

LRESULT HandleSkinSizingMessage(HWND hwnd, LPCALC lpCalc, WPARAM wParam, RECT *prc) {
	LONG ClientOldWidth, ClientOldHeight;
	LONG ClientNewWidth, ClientNewHeight;
	LONG SkinWidth, SkinHeight;

	RECT ClientNewRect;
	CopyRect(&ClientNewRect, prc);
	UnadjustWindowRect(&ClientNewRect, WS_CAPTION | WS_TILEDWINDOW, GetMenu(hwnd) != NULL);
	
	RECT ClientOldRect;
	GetClientRect(hwnd, &ClientOldRect);

	// don't allow resizing from the sides
	if (wParam == WMSZ_LEFT || wParam == WMSZ_RIGHT
		|| wParam == WMSZ_TOP || wParam == WMSZ_BOTTOM) 
	{
		RECT rc;
		GetWindowRect(hwnd, &rc);
		memcpy(prc, &rc, sizeof(RECT));
		return 1;
	}
	
	ClientOldWidth = ClientOldRect.right - ClientOldRect.left;
	ClientOldHeight = ClientOldRect.bottom - ClientOldRect.top;

	SkinWidth = SKIN_WIDTH;
	SkinHeight = SKIN_HEIGHT;

	ClientNewWidth = ClientNewRect.right - ClientNewRect.left;
	ClientNewHeight = ClientNewRect.bottom - ClientNewRect.top;

	LONG AdjustWidth = ClientNewWidth - ClientOldWidth;
	LONG AdjustHeight = ClientNewHeight - ClientOldHeight;

	double SkinRatio = (double)SkinWidth / SkinHeight;
	LONG DiffWidth = 0;
	LONG DiffHeight = 0;
	if (AdjustWidth > AdjustHeight) {
		DiffHeight = (SkinHeight * ClientNewWidth / SkinWidth) - ClientNewHeight;	
	} else if (AdjustHeight > AdjustWidth) {
		DiffWidth = (SkinWidth * ClientNewHeight / SkinHeight) - ClientNewWidth;
	}

	double width_scale = (double)(ClientNewWidth + DiffWidth) / SkinWidth;
	double height_scale = (double)(ClientNewHeight + DiffHeight) / SkinHeight;
	lpCalc->skin_scale = min(width_scale, height_scale) * lpCalc->default_skin_scale;

	if (lpCalc->skin_scale >= lpCalc->default_skin_scale - SKIN_SCALE_SNAP &&
		lpCalc->skin_scale <= lpCalc->default_skin_scale + SKIN_SCALE_SNAP)
	{
		lpCalc->skin_scale = lpCalc->default_skin_scale;
		DiffWidth = SkinWidth - ClientNewWidth;
		DiffHeight = SkinHeight - ClientNewHeight;
	}

	switch (wParam) {
	case WMSZ_TOPLEFT:
	case WMSZ_TOP:
	case WMSZ_TOPRIGHT:
		prc->top -= DiffHeight;
		break;
	default:
		prc->bottom += DiffHeight;
		break;
	}

	switch (wParam) {
	case WMSZ_BOTTOMLEFT:
	case WMSZ_LEFT:
	case WMSZ_TOPLEFT:
		prc->left -= DiffWidth;
		break;
	default:
		prc->right += DiffWidth;
		break;
	}

	InvalidateRect(hwnd, NULL, TRUE);
	return 1;
}

LRESULT GetMinMaxInfo(HWND hwnd, LPCALC lpCalc, MINMAXINFO *info) {
	if (lpCalc == NULL) {
		return 0;
	}

	if (!lpCalc->bSkinEnabled) {
		return 0;
	}

	RECT minRc = { 0, 0, MIN_SKIN_WIDTH, MIN_SKIN_HEIGHT };
	RECT maxRc = { 0, 0, MAX_SKIN_WIDTH, MAX_SKIN_HEIGHT };

	int screenHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	int maxWidth = maxRc.right - maxRc.left;
	int maxHeight = maxRc.bottom - maxRc.top;
	if (screenHeight < maxHeight) {
		maxRc.bottom = screenHeight;
		maxRc.right = SKIN_WIDTH * screenHeight / SKIN_HEIGHT;
	}

	AdjustWindowRect(&minRc, WS_CAPTION | WS_TILEDWINDOW, GetMenu(hwnd) != NULL);
	AdjustWindowRect(&maxRc, WS_CAPTION | WS_TILEDWINDOW, GetMenu(hwnd) != NULL);

	info->ptMinTrackSize.x = minRc.right - minRc.left;
	info->ptMinTrackSize.y = minRc.bottom - minRc.top;
	info->ptMaxTrackSize.x = maxRc.right - maxRc.left;
	info->ptMaxTrackSize.y = maxRc.bottom - maxRc.top;
	return 0;
}