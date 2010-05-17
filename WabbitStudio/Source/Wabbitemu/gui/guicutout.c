#include "stdafx.h"

#include "calc.h"
#include "guifaceplate.h"

extern HINSTANCE g_hInst;
/*static POINT ptRgnEdge[] = {{127,7},
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
							{148,678}};*/

static LRESULT CALLBACK SmallButtonProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		return 0;

	case WM_PAINT:
		{
			BOOL fDown = (BOOL) GetWindowLong(hwnd, 0);

			TCHAR szWindowName[256];
			GetWindowText(hwnd, szWindowName, ARRAYSIZE(szWindowName));

			PAINTSTRUCT ps = {0};
			HDC hdc = BeginPaint(hwnd, &ps);

			RECT rc;
			GetClientRect(hwnd, &rc);
			FillRect(hdc, &rc, GetStockBrush(WHITE_BRUSH));

			if (!calcs[gslot].bCutout)
				return 0;
			
			HBITMAP hbmButtons = LoadBitmap(g_hInst, "close");
			HDC hdcButtons = CreateCompatibleDC(hdc);
			SelectObject(hdcButtons, hbmButtons);

			UINT col, row;
			col = 0;
			if (_tcsicmp(szWindowName, _T("wabbitminimize")) == 0) {
				col = 13;
			}
			row = 0;
			if (fDown == TRUE) {
				row = 13;
			}

			RECT r;
			GetWindowRect(hwnd, &r);
			POINT p;
			p.x = r.left;
			p.y = r.top;

			ScreenToClient(hwnd, &p);
			BitBlt(hdc,0,0,13,13,calcs[gslot].hdcSkin,p.x,p.y,SRCCOPY);

			BLENDFUNCTION bf;
			bf.BlendOp = AC_SRC_OVER;
			bf.BlendFlags = 0;
			bf.SourceConstantAlpha = 160;
			bf.AlphaFormat = 0;
			AlphaBlend(hdc, 0, 0, 13, 13, hdcButtons, col, row, 13, 13, bf );

			DeleteDC(hdcButtons);
			DeleteObject(hbmButtons);

			EndPaint(hwnd, &ps);

			return 0;
		}

	case WM_LBUTTONDOWN:
		{
			
			SetWindowLong(hwnd, 0, (LONG) TRUE);
			SetCapture(hwnd);
			InvalidateRect(hwnd, NULL, FALSE);
			UpdateWindow(hwnd);
			return 0;
		}
	case WM_LBUTTONUP:
		{
			TCHAR szWindowName[256];
			GetWindowText(hwnd, szWindowName, ARRAYSIZE(szWindowName));

			if (_tcsicmp(szWindowName, _T("wabbitminimize")) == 0) {
				ShowWindow(calcs[gslot].hwndFrame, SW_MINIMIZE);
			} else if (_tcsicmp(szWindowName, _T("wabbitclose")) == 0) {
				PostQuitMessage(0);
			}
			SetWindowLong(hwnd, 0, (LONG) FALSE);
			ReleaseCapture();
			InvalidateRect(hwnd, NULL, FALSE);
			UpdateWindow(hwnd);
			return 0;
		}

	case WM_NCCALCSIZE:
		return 0;

	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}


/* Using a preset list of points, cut around the edges to make the
 * frame window transparent.  Also create buttons to allow minimize
 * and close while in skin mode
 */
int EnableCutout(HWND hwndFrame, HBITMAP hbmSkin) {
	if (hwndFrame == NULL) return 1;
	if (calcs[gslot].SkinEnabled == FALSE)
		return 1;

	SetWindowLong(hwndFrame, GWL_EXSTYLE, WS_EX_LAYERED);
	SetWindowLong(hwndFrame, GWL_STYLE, WS_VISIBLE | WS_POPUP);

	int scale = calcs[gslot].Scale;
	if (calcs[gslot].SkinEnabled)
		scale = 2;

	DestroyWindow(calcs[gslot].hwndLCD);
	BOOL disableTransition = TRUE;
	//DwmSetWindowAttribute(calcs[gslot].hwndLCD, DWMWA_TRANSITIONS_FORCEDISABLED, &disableTransition, sizeof(BOOL));
	calcs[gslot].hwndLCD = CreateWindowEx(
			0,
			g_szLCDName,
			"Wabbitemu",
			WS_VISIBLE,
			0, 0, calcs[gslot].cpu.pio.lcd->width*scale, 64*scale,
			calcs[gslot].hwndFrame, NULL, g_hInst,  NULL);

	//SetParent(calcs[gslot].hwndLCD, NULL);
	SetWindowTheme(calcs[gslot].hwndLCD, (LPCWSTR)TEXT (" "), (LPCWSTR)TEXT (" "));

	HDC hScreen = GetDC(NULL);
	BITMAP bmpInfo;

	HBITMAP bmpGray = LoadBitmap(g_hInst, CalcModelTxt[calcs[gslot].model]);
	GetObject(bmpGray, sizeof(BITMAP), &bmpInfo);

	int width = bmpInfo.bmWidth;
	int height = bmpInfo.bmHeight;
	BITMAPFILEHEADER   bmfHeader;
	BITMAPINFOHEADER   bi;
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = width;
	bi.biHeight = height;
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;
	DWORD dwBmpSize = ((width * bi.biBitCount + 31) / 32) * 4 * height;
	BYTE *lpbitmap = (BYTE*)malloc(dwBmpSize);

	// Gets the "bits" from the bitmap and copies them into a buffer
	// which is pointed to by lpbitmap.
	GetDIBits(calcs[gslot].hdcSkin, hbmSkin, 0,
		(UINT)height,
		lpbitmap,
		(BITMAPINFO *)&bi, DIB_RGB_COLORS);

	BYTE* pPixel = lpbitmap;
	HRGN rgn = GetRegion();
	int x,y;
	for(y = 0; y < height; y++)
	{
		for(x = 0; x < width; x++)
		{
			if (PtInRegion(rgn, x, height - y))
			{
				pPixel[3] = 0xFF;
			}
			pPixel+=4;
		}
	}

	SetDIBits(calcs[gslot].hdcSkin, hbmSkin, 0,
			(UINT)height,
			lpbitmap,
			(BITMAPINFO *)&bi, DIB_RGB_COLORS);

	RECT rc;
	GetClientRect(hwndFrame, &rc);
	POINT rectTopLeft;
	rectTopLeft.x = rc.left;
	rectTopLeft.y = rc.top;

	BLENDFUNCTION bf;
	bf.BlendOp = AC_SRC_OVER;
	bf.BlendFlags = 0;
	bf.SourceConstantAlpha = 255;
	bf.AlphaFormat = AC_SRC_ALPHA;

	POINT ptSrc = {0 , 0};
	SIZE size;
	size.cx = bmpInfo.bmWidth;
	size.cy = bmpInfo.bmHeight;
	SetRect(&calcs[gslot].rectSkin, 0, 0, size.cx, size.cy);
	SetBkColor(calcs[gslot].hdcSkin, 0xFFFFFF);

	//AlphaBlend(hMemory, 0, 0, calcs[gslot].rectSkin.right, calcs[gslot].rectSkin.bottom, hBackground,
	//					calcs[gslot].rectSkin.left, calcs[gslot].rectSkin.top, calcs[gslot].rectSkin.right, calcs[gslot].rectSkin.bottom, bf);

	int done = UpdateLayeredWindow(hwndFrame, hScreen, NULL, &size, calcs[gslot].hdcSkin, &ptSrc, RGB(255,255,255), &bf, ULW_ALPHA);
	int error;
	if (!done)
		error = GetLastError();

	DeleteObject(bmpGray);
	ReleaseDC(0, hScreen);
	UpdateWindow(calcs[gslot].hwndLCD);
	SendMessage(hwndFrame, WM_MOVE, 0, 0);
	//return 0;

	/*unsigned int nPoints = (sizeof(ptRgnEdge) / sizeof(POINT)) * 2;
	POINT ptRgn[nPoints];

	// Copy points and their reverses to the new array
	memcpy(ptRgn, ptRgnEdge, (nPoints / 2) * sizeof(POINT));

	int i;
	for (i = nPoints/2; i < nPoints; i++) {
		ptRgn[i].x = 310 - ptRgnEdge[nPoints - i - 1].x;
		ptRgn[i].y = ptRgnEdge[nPoints - i - 1].y;
	}*/

	// If there's a menu bar, include its height in the skin offset
	HMENU hmenu = GetMenu(hwndFrame);
	int cyMenu;
	if (hmenu == NULL) {
		cyMenu = 0;
	} else {
		cyMenu = GetSystemMetrics(SM_CYMENU);
	}

	// Add in constant adjust factors
	/*for (i = 0; i < nPoints; i++) {
		ptRgn[i].x += GetSystemMetrics(SM_CXFIXEDFRAME);
		ptRgn[i].y += cyMenu + GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYFIXEDFRAME);
	}

	HRGN hrgn = CreatePolygonRgn(ptRgn, nPoints, WINDING);
	if (hrgn == NULL) return 1;

	//int result = SetWindowRgn(hwndFrame, hrgn, TRUE);
	int result = SetWindowRgn(hwndFrame, NULL, TRUE);
	if (result == 0) return 1;*/

	// Create the two buttons that appear when the skin is cutout
	WNDCLASS wc = {0};
	wc.cbWndExtra = sizeof(BOOL) + sizeof(int);
	wc.lpfnWndProc = SmallButtonProc;
	wc.lpszClassName = _T("WABBITSMALLBUTTON");
	wc.hInstance = g_hInst;
	RegisterClass(&wc);

	HWND hwndButton;
	hwndButton = CreateWindow(
		"WABBITSMALLBUTTON",
		"wabbitclose",
		WS_VISIBLE, // | BS_OWNERDRAW,
		270, 19,
		13, 13,
		hwndFrame,
		(HMENU) NULL,
		g_hInst,
		NULL);
	if (hwndButton == NULL) return 1;

	SetWindowLong(hwndButton, GWL_STYLE, WS_VISIBLE);

	hwndButton = CreateWindowEx(
		0,
		"WABBITSMALLBUTTON",
		"wabbitminimize",
		WS_VISIBLE, // | BS_OWNERDRAW,
		254, 19,
		13, 13,
		hwndFrame,
		(HMENU) NULL,
		g_hInst,
		NULL);
	if (hwndButton == NULL) return 1;

	SetWindowLong(hwndButton, GWL_STYLE, WS_VISIBLE);

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
	BOOL disableTransition = TRUE;
	//DwmSetWindowAttribute(calcs[gslot].hwndLCD, DWMWA_TRANSITIONS_FORCEDISABLED, &disableTransition, sizeof(BOOL));

	int scale = calcs[gslot].Scale;
	if (calcs[gslot].SkinEnabled)
		scale = 2;
	DestroyWindow(calcs[gslot].hwndLCD);
	calcs[gslot].hwndLCD = CreateWindowEx(
			0,
			g_szLCDName,
			"LCD",
			WS_VISIBLE |  WS_CHILD,
			0, 0, calcs[gslot].cpu.pio.lcd->width* scale, 64*scale,
			calcs[gslot].hwndFrame, (HMENU) 99, g_hInst,  NULL);

	SetWindowLong(hwndFrame, GWL_EXSTYLE, 0);
	SetWindowLong(hwndFrame, GWL_STYLE, (WS_TILEDWINDOW |  WS_VISIBLE | WS_CLIPCHILDREN) & ~(WS_MAXIMIZEBOX /* | WS_SIZEBOX */));

	if (calcs[gslot].SkinEnabled == FALSE) {

		//SetRect(&rectSkin, 0, 0, 314, 688);

		// If there's a menu bar, include its height in the skin offset
		HMENU hmenu = GetMenu(hwndFrame);
		int cyMenu;
		if (hmenu == NULL) {
			cyMenu = 0;
		} else {
			cyMenu = GetSystemMetrics(SM_CYMENU);
		}

		/*RECT wr;
		GetWindowRect(hwndFrame, &wr);
		SetWindowPos(hwndFrame, NULL,
				wr.left + GetSystemMetrics(SM_CXFIXEDFRAME) - 8,
				wr.top + (cyMenu + GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYFIXEDFRAME)),
				0, 0,
				SWP_NOZORDER|SWP_NOSIZE);*/
	}

//	SetWindowRgn(hwndFrame, NULL, TRUE);

	HWND hwndButton;

	hwndButton = FindWindowEx(NULL, NULL, "WABBITSMALLBUTTON", "wabbitclose");
	if (hwndButton) DestroyWindow(hwndButton);

	hwndButton = FindWindowEx(NULL, NULL, "WABBITSMALLBUTTON", "wabbitminimize");
	if (hwndButton) DestroyWindow(hwndButton);

	InvalidateRect(hwndFrame, NULL, TRUE);
	return 0;
}
