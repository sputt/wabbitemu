#include <windows.h>
#include "calc.h"

extern HINSTANCE g_hInst;

static POINT ptRgnEdge[] = {{127,7},
							{64,9},
							{42,10},
							{22,12},
							{17,14},	
							{16,15},	
							{11,23},
							{9, 177},
							{8, 225},
							{8, 302},
							{9, 398},	
							{15,615},
							{17,628},
							{23,641},
							{37,655},
							{44,659},
							{67,668},
							{124,677},
							{148,678}};

/* Using a preset list of points, cut around the edges to make the
 * frame window transparent.  Also create buttons to allow minimize
 * and close while in skin mode
 */
int EnableCutout(HWND hwndFrame) {
	if (hwndFrame == NULL) return 1;
	if (calcs[gslot].SkinEnabled == FALSE)
		return 1;
	
	
	
	unsigned int nPoints = (sizeof(ptRgnEdge) / sizeof(POINT)) * 2;
	POINT ptRgn[(sizeof(ptRgnEdge) / sizeof(POINT)) * 2];
	
	// Copy points and their reverses to the new array
	memcpy(ptRgn, ptRgnEdge, (nPoints / 2) * sizeof(POINT));
	
	int i;
	for (i = nPoints/2; i < nPoints; i++) {
		ptRgn[i].x = 310 - ptRgnEdge[nPoints - i - 1].x;
		ptRgn[i].y = ptRgnEdge[nPoints - i - 1].y;
	}
	
	// If there's a menu bar, include its height in the skin offset
	HMENU hmenu = GetMenu(hwndFrame);
	
	int cyMenu;
	if (hmenu == NULL) {
		cyMenu = 0;
	} else {
		cyMenu = GetSystemMetrics(SM_CYMENU);
	}
	
	// Add in constant adjust factors
	for (i = 0; i < nPoints; i++) {
		ptRgn[i].x += GetSystemMetrics(SM_CXFIXEDFRAME);
		ptRgn[i].y += cyMenu + GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYFIXEDFRAME);
	}

	HRGN hrgn = CreatePolygonRgn(ptRgn, nPoints, WINDING);
	if (hrgn == NULL) return 1;
	
	int result = SetWindowRgn(hwndFrame, hrgn, TRUE);	
	if (result == 0) return 1;
	
	// Create the two buttons that appear when the skin is cutout
	HWND hwndButton;
	hwndButton = CreateWindow(
		"BUTTON",
		"close",
		WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
		270, 19,
		13, 13,
		hwndFrame,
		(HMENU) 0,
		g_hInst,
		NULL);
	if (hwndButton == NULL) return 1;

	hwndButton = CreateWindowEx(
		0,
		"BUTTON",
		"minimize",
		WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
		254, 19,
		13, 13,
		hwndFrame,
		(HMENU) 1,
		g_hInst,
		NULL);
	if (hwndButton == NULL) return 1;
	
	if (calcs[gslot].SkinEnabled == FALSE) {
		RECT wr;
		GetWindowRect(hwndFrame, &wr);
		SetWindowPos(hwndFrame, NULL, 
				wr.left - GetSystemMetrics(SM_CXFIXEDFRAME) - 8,
				wr.top - (cyMenu + GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYFIXEDFRAME)),
				0, 0,
				SWP_NOZORDER|SWP_NOSIZE);
	}
	
	InvalidateRect(hwndFrame, NULL, TRUE);
	return 0;
}

/* Remove the cutout rgn from the window and delete
 * the small minimize and close buttons
 */
int DisableCutout(HWND hwndFrame) {
	if (calcs[gslot].SkinEnabled == FALSE) {
		// If there's a menu bar, include its height in the skin offset
		HMENU hmenu = GetMenu(hwndFrame);
		int cyMenu;
		if (hmenu == NULL) {
			cyMenu = 0;
		} else {
			cyMenu = GetSystemMetrics(SM_CYMENU);
		}
		
		RECT wr;
		GetWindowRect(hwndFrame, &wr);
		SetWindowPos(hwndFrame, NULL, 
				wr.left + GetSystemMetrics(SM_CXFIXEDFRAME) - 8,
				wr.top + (cyMenu + GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYFIXEDFRAME)),
				0, 0,
				SWP_NOZORDER|SWP_NOSIZE);
	}
	
	SetWindowRgn(hwndFrame, NULL, TRUE);
	
	HWND hwndButton;
	
	hwndButton = FindWindowEx(hwndFrame, NULL, "BUTTON", "close");
	if (hwndButton) DestroyWindow(hwndButton);
	
	hwndButton = FindWindowEx(hwndFrame, NULL, "BUTTON", "minimize");
	if (hwndButton) DestroyWindow(hwndButton);
	
	InvalidateRect(hwndFrame, NULL, TRUE);
	return 0;
}
