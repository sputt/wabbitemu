#include "stdafx.h"

#include "gui.h"
#include "guifaceplate.h"
#include "guibuttons.h"
#include "resource.h"

#define IDC_SMALLCLOSE		45
#define IDC_SMALLMINIMIZE	46

extern HINSTANCE g_hInst;

static LRESULT CALLBACK SmallButtonProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_CREATE:
			return 0;

		case WM_PAINT: {
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
			
				HBITMAP hbmButtons = LoadBitmap(g_hInst, _T("close"));
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

		case WM_LBUTTONDOWN: {
				SetWindowLong(hwnd, 0, (LONG) TRUE);
				SetCapture(hwnd);
				InvalidateRect(hwnd, NULL, FALSE);
				UpdateWindow(hwnd);
				return 0;
		}
		case WM_LBUTTONUP: {
				TCHAR szWindowName[256];
				GetWindowText(hwnd, szWindowName, ARRAYSIZE(szWindowName));

				if (_tcsicmp(szWindowName, _T("wabbitminimize")) == 0) {
					ShowWindow(calcs[gslot].hwndFrame, SW_MINIMIZE);
				} else if (_tcsicmp(szWindowName, _T("wabbitclose")) == 0) {
					SendMessage(calcs[gslot].hwndFrame, WM_CLOSE, 0, 0);
				}
				SetWindowLong(hwnd, 0, (LONG) FALSE);
				ReleaseCapture();
				InvalidateRect(hwnd, NULL, FALSE);
				UpdateWindow(hwnd);
				return 0;
		}
		case WM_KEYDOWN:
			HandleKeyDown((unsigned int) wParam);
			return 0;
		case WM_KEYUP:
			HandleKeyUp((unsigned int) wParam);
			return 0;
		case WM_CLOSE:
			SendMessage(calcs[gslot].hwndFrame, uMsg, wParam, lParam);
		case WM_NCCALCSIZE:
			return 0;
		default:
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}

void PositionLittleButtons(HWND hwnd)
{
	calc_t *lpCalc = (calc_t *) GetWindowLongPtr(hwnd, GWLP_USERDATA);
	HDWP hdwp = BeginDeferWindowPos(3);
	RECT wr;
	GetWindowRect(hwnd, &wr);
	DeferWindowPos(hdwp, lpCalc->hwndSmallMinimize, NULL, wr.left + 285, wr.top + 34, 13, 13, 0);
	DeferWindowPos(hdwp, lpCalc->hwndSmallClose, NULL, wr.left + 300, wr.top + 34, 13, 13, 0);
	EndDeferWindowPos(hdwp);
}

typedef HRESULT (WINAPI *SetThemeFunc)(HWND, LPCWSTR, LPCWSTR);
typedef HRESULT (WINAPI *DwmSetAttrib)(HWND, DWORD, LPCVOID, DWORD);

/* Using a preset list of points, cut around the edges to make the
 * frame window transparent.  Also create buttons to allow minimize
 * and close while in skin mode
 */
int EnableCutout(HWND hwndFrame, HBITMAP hbmSkin) {
	if (hwndFrame == NULL) return 1;
	calc_t *lpCalc = (calc_t *) GetWindowLongPtr(hwndFrame, GWLP_USERDATA);
	if (lpCalc->SkinEnabled == FALSE)
		return 1;

	u_int width = lpCalc->rectSkin.right;
	u_int height = lpCalc->rectSkin.bottom;

	int scale = lpCalc->Scale;
	if (lpCalc->SkinEnabled)
		scale = 2;

	DestroyWindow(lpCalc->hwndLCD);
	//BOOL disableTransition = TRUE;
	//DwmSetWindowAttribute(lpCalc->hwndLCD, DWMWA_TRANSITIONS_FORCEDISABLED, &disableTransition, sizeof(BOOL));
	HMODULE hasDWM = LoadLibrary(_T("dwmapi.dll"));
	if (hasDWM) {
		BOOL disableTransition = TRUE;
		DwmSetAttrib SetAttrib = (DwmSetAttrib) GetProcAddress(hasDWM, "DwmSetWindowAttribute");
		SetAttrib(lpCalc->hwndLCD, DWMWA_TRANSITIONS_FORCEDISABLED, &disableTransition, sizeof(BOOL));
		FreeLibrary(hasDWM);
	}
	lpCalc->hwndLCD = CreateWindowEx(
			0,
			g_szLCDName,
			_T("Wabbitemu"),
			WS_VISIBLE,
			0, 0, lpCalc->cpu.pio.lcd->width * scale, 64 * scale,
			hwndFrame, NULL, g_hInst,  (LPVOID *) lpCalc);

	SetWindowTheme(lpCalc->hwndLCD, (LPCWSTR) _T(" "), (LPCWSTR) _T(" "));
	HDC hScreen = GetDC(NULL);

	if (lpCalc->model == TI_84PSE) {
			BITMAPINFOHEADER bih;
			bih.biSize = sizeof(BITMAPINFOHEADER);
			bih.biWidth = width;
			bih.biHeight = height;
			bih.biPlanes = 1;
			bih.biBitCount = 32;
			bih.biCompression = BI_RGB;
			bih.biSizeImage = 0;
			bih.biXPelsPerMeter = 0;
			bih.biYPelsPerMeter = 0;
			bih.biClrUsed = 0;
			bih.biClrImportant = 0;
			DWORD dwBmpSize = ((width * bih.biBitCount + 31) / 32) * 4 * height;
			BITMAPINFO bi;
			bi.bmiHeader = bih;
			BYTE *bitmap = (BYTE*) malloc(dwBmpSize);
		
			// Gets the "bits" from the bitmap and copies them into a buffer
			// which is pointed to by lpbitmap.
			GetDIBits(lpCalc->hdcSkin, hbmSkin, 0,
				height,
				bitmap,
				&bi, DIB_RGB_COLORS);

			//this really sucked to figure out, but basically you can't fuck with
			//the alpha channel in a bitmap unless you use GetDIBits to get it
			//in an array, change the highest byte, then reset with SetDIBits
			//This colors the faceplate that way
			BYTE* pPixel = bitmap;
			HRGN rgn = GetRegion();
			unsigned int x, y;
			for(y = 0; y < height; y++) {
				for(x = 0; x < width; x++) {
					if (PtInRegion(rgn, x, height - y))
						pPixel[3] = 0xFF;
					pPixel+=4;
				}
			}

			SetDIBitsToDevice(lpCalc->hdcSkin, 0, 0, width, height, 0, 0, 0,
					height,
					bitmap,
					&bi, DIB_RGB_COLORS);
			//SelectObject(lpCalc->hdcSkin, hbmSkin);
			free(bitmap);
	}

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
	size.cx = width;
	size.cy = height;
	SetBkColor(lpCalc->hdcSkin, 0xFF0000);

	SetWindowLongPtr(hwndFrame, GWL_EXSTYLE, WS_EX_LAYERED);
	SetWindowLongPtr(hwndFrame, GWL_STYLE, WS_VISIBLE | WS_POPUP);

	int done = UpdateLayeredWindow(hwndFrame, hScreen, NULL, &size, lpCalc->hdcSkin, &ptSrc, RGB(255,255,255), &bf, ULW_ALPHA);
	int error;
	if (!done)
		error = GetLastError();

	ReleaseDC(0, hScreen);
	UpdateWindow(lpCalc->hwndLCD);
	SendMessage(hwndFrame, WM_MOVE, 0, 0);

	// If there's a menu bar, include its height in the skin offset
	HMENU hmenu = GetMenu(hwndFrame);
	int cyMenu;
	if (hmenu == NULL) {
		cyMenu = 0;
	} else {
		cyMenu = GetSystemMetrics(SM_CYMENU);
	}

	// Create the two buttons that appear when the skin is cutout
	WNDCLASS wc = {0};
	wc.cbWndExtra = sizeof(bool) + sizeof(int);
	wc.lpfnWndProc = SmallButtonProc;
	wc.lpszClassName = _T("WABBITSMALLBUTTON");
	wc.hInstance = g_hInst;
	RegisterClass(&wc);

	lpCalc->hwndSmallClose = CreateWindow(
		_T("WABBITSMALLBUTTON"),
		_T("wabbitclose"),
		WS_VISIBLE, // | BS_OWNERDRAW,
		270, 19,
		13, 13,
		hwndFrame,
		(HMENU) NULL,
		g_hInst,
		NULL);
	if (lpCalc->hwndSmallClose == NULL) return 1;
	SetWindowLong(lpCalc->hwndSmallClose, GWL_STYLE, WS_VISIBLE);

	lpCalc->hwndSmallMinimize = CreateWindowEx(
		0,
		_T("WABBITSMALLBUTTON"),
		_T("wabbitminimize"),
		WS_VISIBLE, // | BS_OWNERDRAW,
		254, 19,
		13, 13,
		hwndFrame,
		(HMENU) NULL,
		g_hInst,
		NULL);
	if (lpCalc->hwndSmallMinimize == NULL) return 1;
	SetWindowLong(lpCalc->hwndSmallMinimize, GWL_STYLE, WS_VISIBLE);

	if (!lpCalc->SkinEnabled) {
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
	HMODULE hasDWM = LoadLibrary(_T("dwmapi.dll"));
	calc_t *lpCalc = (calc_t *) GetWindowLongPtr(hwndFrame, GWLP_USERDATA);
	if (hasDWM) {
		BOOL disableTransition = TRUE;
		DwmSetAttrib SetAttrib = (DwmSetAttrib) GetProcAddress(hasDWM, "DwmSetWindowAttribute");
		if (SetAttrib != NULL)
		{
			SetAttrib(lpCalc->hwndLCD, DWMWA_TRANSITIONS_FORCEDISABLED, &disableTransition, sizeof(BOOL));
		}
		FreeLibrary(hasDWM);
	}
	//BOOL disableTransition = TRUE;
	//DwmSetWindowAttribute(lpCalc->hwndLCD, DWMWA_TRANSITIONS_FORCEDISABLED, &disableTransition, sizeof(BOOL));

	int scale = lpCalc->Scale;
	if (lpCalc->SkinEnabled)
		scale = 2;
	if (lpCalc->hwndLCD)
		DestroyWindow(lpCalc->hwndLCD);
	lpCalc->hwndLCD = CreateWindowEx(
			0,
			g_szLCDName,
			_T("LCD"),
			WS_VISIBLE |  WS_CHILD,
			0, 0, lpCalc->cpu.pio.lcd->width*scale, 64*scale,
			hwndFrame, (HMENU) 99, g_hInst,  (LPVOID) GetWindowLongPtr(hwndFrame, GWLP_USERDATA));

	SetWindowLong(hwndFrame, GWL_EXSTYLE, 0);
	SetWindowLong(hwndFrame, GWL_STYLE, (WS_TILEDWINDOW |  WS_VISIBLE | WS_CLIPCHILDREN) & ~(WS_MAXIMIZEBOX /* | WS_SIZEBOX */));

	if (!lpCalc->SkinEnabled) {
		// If there's a menu bar, include its height in the skin offset
		HMENU hmenu = GetMenu(hwndFrame);
		int cyMenu;
		if (hmenu == NULL) {
			cyMenu = 0;
		} else {
			cyMenu = GetSystemMetrics(SM_CYMENU);
		}
	}

	if (lpCalc->hwndSmallClose) DestroyWindow(lpCalc->hwndSmallClose);

	if (lpCalc->hwndSmallMinimize) DestroyWindow(lpCalc->hwndSmallMinimize);

	InvalidateRect(hwndFrame, NULL, TRUE);
	return 0;
}
