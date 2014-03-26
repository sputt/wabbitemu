#include "stdafx.h"

#include "guisize.h"
#include "gui.h"
#include "guiskin.h"
#include "guicutout.h"

#define SKIN_SCALE_SNAP .025

unsigned int GetDefaultKeymapScale(int model) {
	switch (model) {
	case TI_84PCSE:
		return DEFAULT_84PCSE_KEYMAP_SCALE;
	default:
		return DEFAULT_KEYMAP_SCALE;
	}
}

LRESULT HandleMoveMessage(HWND hwnd, LPMAINWINDOW lpMainWindow) {
	if (lpMainWindow->bCutout && lpMainWindow->bSkinEnabled) {
		UINT left = (LONG)(lpMainWindow->m_RectLCD.X * lpMainWindow->skin_scale);
		UINT top = (LONG)(lpMainWindow->m_RectLCD.Y * lpMainWindow->skin_scale);

		HDWP hdwp = BeginDeferWindowPos(3);
		RECT rc;
		GetWindowRect(hwnd, &rc);
		OffsetRect(&rc, left, top);
		DeferWindowPos(hdwp, lpMainWindow->hwndLCD, HWND_TOP, rc.left, rc.top, 0, 0, SWP_NOSIZE);
		EndDeferWindowPos(hdwp);
		PositionLittleButtons(hwnd, lpMainWindow);
	}

	return 0;
}

LRESULT HandleSizeMessage(HWND hwnd, HWND hwndLcd, LPMAINWINDOW lpMainWindow, LPCALC lpCalc,
	BOOL isSkinEnabled, BOOL isCutout)
{
	if (lpCalc == NULL) {
		return 0;
	}

	u_int width;
	RECT clientRect;

	UINT default_scale = GetDefaultKeymapScale(lpCalc->model);
	UINT scale = isSkinEnabled ? default_scale : max(lpMainWindow->scale, default_scale);
	LONG lcdWidth, lcdHeight;
	if (isSkinEnabled) {
		lcdWidth = lpMainWindow->m_RectLCD.Width / default_scale;
		lcdHeight = lpMainWindow->m_RectLCD.Height / default_scale;
		lcdWidth = (LONG)(lcdWidth * lpMainWindow->skin_scale);
		lcdHeight = (LONG)(lcdHeight *lpMainWindow->skin_scale);
	} else {
		LCDBase_t *lcd = lpCalc->cpu.pio.lcd;
		if (lcd == NULL) {
			return 1;
		}

		lcdWidth = lcd->display_width;
		lcdHeight = lcd->height;
	}

	lcdWidth *= scale;
	lcdHeight *= scale;

	GetClientRect(hwnd, &clientRect);
	width = clientRect.right - clientRect.left;

	POINT lcdPoint;
	if (!isSkinEnabled) {
		lcdPoint.x = 0;
		lcdPoint.y = 0;
		if (width > (u_int) lcdWidth) {
			// if the lcd is less than client, center the lcd
			lcdPoint.x += (width - lcdWidth) / 2;
		}
	} else if (isCutout) {
		UINT left = (LONG)(lpMainWindow->m_RectLCD.GetLeft() * lpMainWindow->skin_scale);
		UINT top = (LONG)(lpMainWindow->m_RectLCD.GetTop() * lpMainWindow->skin_scale);

		RECT rc;
		GetWindowRect(hwnd, &rc);
		lcdPoint.x = left + rc.left;
		lcdPoint.y = top + rc.top;
	} else {
		lcdPoint.x = (LONG)(lpMainWindow->m_RectLCD.GetLeft() * lpMainWindow->skin_scale);
		lcdPoint.y = (LONG)(lpMainWindow->m_RectLCD.GetTop() * lpMainWindow->skin_scale);
	}

	MoveWindow(hwndLcd, lcdPoint.x, lcdPoint.y, lcdWidth, lcdHeight, TRUE);

	// force little buttons to be correct
	if (lpMainWindow->bCutout && isSkinEnabled) {
		PositionLittleButtons(hwnd, lpMainWindow);
	}

	if (lpMainWindow->hwndStatusBar != NULL) {
		SendMessage(lpMainWindow->hwndStatusBar, WM_SIZE, SIZE_RESTORED, 0);
	}

	UpdateWindow(hwndLcd);
	return 0;
}

LRESULT HandleLCDSizingMessage(HWND hwnd, HWND hwndStatusBar, LPMAINWINDOW lpMainWindow, WPARAM wParam, RECT *prc, LONG lcdWidth) {
	if (lpMainWindow == NULL) {
		return 1;
	}

	LPCALC lpCalc = lpMainWindow->lpCalc;
	if (lpCalc == NULL) {
		return 1;
	}

	LCDBase_t *lcd = lpCalc->cpu.pio.lcd;
	LONG ClientAdjustWidth, ClientAdjustHeight;
	LONG AdjustWidth, AdjustHeight;

	// Adjust for border and menu
	RECT rc = {0, 0, 0, 0};
	AdjustWindowRect(&rc, WS_CAPTION | WS_TILEDWINDOW, GetMenu(hwnd) != NULL);

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


	lpMainWindow->scale = min(cx_mult, cy_mult);

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

double GetSkinScale(LONG ClientNewWidth, LONG ClientNewHeight, 
	LONG *DiffWidth, LONG *DiffHeight, double default_skin_scale)
{
	
	double width_scale = (double)(ClientNewWidth + *DiffWidth) / SKIN_WIDTH;
	double height_scale = (double)(ClientNewHeight + *DiffHeight) / SKIN_HEIGHT;
	double skin_scale = min(width_scale, height_scale) * default_skin_scale;

	if (skin_scale >= default_skin_scale - SKIN_SCALE_SNAP &&
		skin_scale <= default_skin_scale + SKIN_SCALE_SNAP) {
		skin_scale = default_skin_scale;
		*DiffWidth = SKIN_WIDTH - ClientNewWidth;
		*DiffHeight = SKIN_HEIGHT - ClientNewHeight;
	}

	return skin_scale;
}

LRESULT HandleSkinSizingMessage(HWND hwnd, LPMAINWINDOW lpMainWindow, WPARAM wParam, RECT *prc) {
	if (lpMainWindow == NULL) {
		return 1;
	}

	LONG ClientOldWidth, ClientOldHeight;
	LONG ClientNewWidth, ClientNewHeight;

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

	ClientNewWidth = ClientNewRect.right - ClientNewRect.left;
	ClientNewHeight = ClientNewRect.bottom - ClientNewRect.top;

	LONG AdjustWidth = ClientNewWidth - ClientOldWidth;
	LONG AdjustHeight = ClientNewHeight - ClientOldHeight;

	double SkinRatio = (double)SKIN_WIDTH / (double)SKIN_HEIGHT;
	double CurrentRatio = (double)ClientNewWidth / (double)ClientNewHeight;
	LONG DiffWidth = 0;
	LONG DiffHeight = 0;
	if (AdjustWidth > AdjustHeight) {
		DiffHeight = (SKIN_HEIGHT * ClientNewWidth / SKIN_WIDTH) - ClientNewHeight;
	} else if ((AdjustHeight > AdjustWidth) || (CurrentRatio - SkinRatio) > DBL_EPSILON) {
		// second case may be caused by being sent WM_SIZE (eg aero snap)
		DiffWidth = (SKIN_WIDTH * ClientNewHeight / SKIN_HEIGHT) - ClientNewWidth;
	}

	lpMainWindow->skin_scale = GetSkinScale(ClientNewWidth, ClientNewHeight,
		&DiffWidth, &DiffHeight, lpMainWindow->default_skin_scale);

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

LRESULT GetMinMaxInfo(HWND hwnd, LPMAINWINDOW lpMainWindow, MINMAXINFO *info) {
	if (lpMainWindow == NULL) {
		return 0;
	}

	if (!lpMainWindow->bSkinEnabled) {
		return 0;
	}

	RECT minRc = { 0, 0, MIN_SKIN_WIDTH, MIN_SKIN_HEIGHT };
	RECT maxRc = { 0, 0, MAX_SKIN_WIDTH, MAX_SKIN_HEIGHT };

	int screenHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);
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