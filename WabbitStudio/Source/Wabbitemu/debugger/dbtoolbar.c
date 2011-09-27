#include "stdafx.h"

#include "dbtoolbar.h"
#include "resource.h"
#include "guicontext.h"

#include "label.h"
#include "calc.h"

extern HINSTANCE g_hInst;
extern HFONT hfontSegoe;

static WNDPROC OldButtonProc;

HWND hwndLastFocus;


#define FADE_SOLID 10
#define FADE_SPEED 20


#if !USE_GDIPLUS
typedef DWORD ARGB;
#endif


void InitBitmapInfo(BITMAPINFO *pbmi, ULONG cbInfo, LONG cx, LONG cy, WORD bpp)
{
	ZeroMemory(pbmi, cbInfo);
	pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pbmi->bmiHeader.biPlanes = 1;
	pbmi->bmiHeader.biCompression = BI_RGB;

	pbmi->bmiHeader.biWidth = cx;
	pbmi->bmiHeader.biHeight = cy;
	pbmi->bmiHeader.biBitCount = bpp;
}

HRESULT AddBitmapToMenuItem(HMENU hmenu, int iItem, BOOL fByPosition, HBITMAP hbmp)
{
	HRESULT hr = E_FAIL;

	MENUITEMINFO mii = { sizeof(mii) };
	mii.fMask = MIIM_BITMAP;
	mii.hbmpItem = hbmp;
	if (SetMenuItemInfo(hmenu, iItem, fByPosition, &mii)) {
		hr = S_OK;
	}

	return hr;
}

HRESULT ConvertToPARGB32(HDC hdc, ARGB *pargb, HBITMAP hbmp, SIZE sizImage, int cxRow)
{
	BITMAPINFO bmi;
	InitBitmapInfo(&bmi, sizeof(bmi), sizImage.cx, sizImage.cy, 32);

	HRESULT hr = E_OUTOFMEMORY;
	HANDLE hHeap = GetProcessHeap();
	void *pvBits = HeapAlloc(hHeap, 0, bmi.bmiHeader.biWidth * 4 * bmi.bmiHeader.biHeight);
	if (pvBits) {
		hr = E_UNEXPECTED;
		if (GetDIBits(hdc, hbmp, 0, bmi.bmiHeader.biHeight, pvBits, &bmi, DIB_RGB_COLORS) == bmi.bmiHeader.biHeight) {
			ULONG cxDelta = cxRow - bmi.bmiHeader.biWidth;
			ARGB *pargbMask = (ARGB*)pvBits;
			ULONG y;
			for (y = bmi.bmiHeader.biHeight; y; y--) {
				ULONG x;
				for (x = bmi.bmiHeader.biWidth; x; x--) {
					if (*pargbMask++) {
						// transparent pixel
						*pargb++ = 0;
					} else {
						// opaque pixel
						*pargb++ |= 0xFF000000;
					}
				}
				pargb += cxDelta;
			}
			hr = S_OK;
		}
		HeapFree(hHeap, 0, pvBits);
	}

	return hr;
}

BOOL HasAlpha(ARGB *pargb, SIZE sizImage, int cxRow)
{
	ULONG cxDelta = cxRow - sizImage.cx;
	ULONG y;
	for (y = sizImage.cy; y; y--) {
		ULONG x;
		for (x = sizImage.cx; x; x--) {
			if (*pargb++ & 0xFF000000) {
				return TRUE;
			}
		}

		pargb += cxDelta;
	}

	return FALSE;
}

void PaintToolbarBackground(HWND hwndToolbar, HDC hdc, LPRECT r) {
	RECT rc;
	GetClientRect(hwndToolbar, &rc);

	HDC hdcBuf = CreateCompatibleDC(hdc);
	HBITMAP hbmBuf = CreateCompatibleBitmap(hdc, r->right - r->left, r->bottom - r->top);
	SelectObject(hdcBuf, hbmBuf);

	HDC hdcRight = CreateCompatibleDC(hdcBuf);
	HBITMAP hbmRight;

	DWORD dwVersion = GetVersion();
	DWORD dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
	BOOL fIsWindows7 = (dwMajorVersion >= 0x06) ? TRUE : FALSE;
	if (IsAppThemed()) {
		if (fIsWindows7 == TRUE)
		{
			hbmRight = LoadBitmap(g_hInst, _T("TBRIGHT7"));
		}
		else
		{
			hbmRight = LoadBitmap(g_hInst, _T("TBRIGHT"));
		}
		SelectObject(hdcRight, hbmRight);

		StretchBlt(hdcBuf, 0, 0, r->right - r->left, r->bottom - r->top,
				hdcRight, 0, r->top, 1, r->bottom - r->top, SRCCOPY);

		DeleteObject(hbmRight);
		DeleteDC(hdcRight);

		// Set up the alpha function for the bitmap with alpha values
		BLENDFUNCTION bf;
		bf.BlendOp = AC_SRC_OVER;
		bf.BlendFlags = 0;
		bf.SourceConstantAlpha = 255;
		bf.AlphaFormat = AC_SRC_ALPHA;

		// Create the header for the bitmap with alpha values
		BITMAPINFO bmi;
		ZeroMemory(&bmi, sizeof(BITMAPINFO));

		BITMAPINFOHEADER *bi = &bmi.bmiHeader;
		bi->biSize = sizeof(BITMAPINFOHEADER);
		bi->biWidth = r->right - r->left;
		bi->biHeight = 1;
		bi->biPlanes = 1;
		bi->biBitCount = 32;
		bi->biCompression = BI_RGB;

		int width = bi->biWidth;
		int height = bi->biHeight;

		if (fIsWindows7 == FALSE)
		{
			HDC hdcGrad = CreateCompatibleDC(hdc);
			// Create a solid brush of the gradient color
			HBRUSH hbrGrad = CreateSolidBrush(RGB(0, 190, 0));
			SelectObject(hdcGrad, hbrGrad);

			SelectObject(hdcGrad, GetStockObject(DC_PEN));
			SetDCPenColor(hdcGrad, RGB(0, 190, 0));

		
			BYTE *pBits;
			HBITMAP hbmGrad = CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (void**) &pBits, NULL, 0);

			SelectObject(hdcGrad, hbmGrad);

			// Fill it with green
			Rectangle(hdcGrad, 0, 0, width, height);

			int x;

			BYTE * pPixel = pBits;
			for (x = r->left; x < r->right; x++, pPixel+=4) {
				pPixel[3] = (BYTE) (255*(x+1)/rc.right/8);

				pPixel[0] = pPixel[0] * pPixel[3] / 0xFF;
				pPixel[1] = pPixel[1] * pPixel[3] / 0xFF;
				pPixel[2] = pPixel[2] * pPixel[3] / 0xFF;

			}

			AlphaBlend(	hdcBuf, 0, 0, r->right - r->left, r->bottom - r->top,
						hdcGrad, 0, 0, r->right - r->left, 1,
						bf);

			DeleteObject(hbrGrad);
			DeleteObject(hbmGrad);
			DeleteDC(hdcGrad);
		}
	} else {
		FillRect(hdcBuf, &rc, GetSysColorBrush(COLOR_MENU));
	}
	BitBlt(hdc, 0, 0, r->right - r->left, r->bottom - r->top, hdcBuf, 0, 0, SRCCOPY);

	DeleteObject(hbmBuf);
	DeleteDC(hdcBuf);
}

VOID CALLBACK ButtonFadeProc(HWND hwnd, UINT Message, UINT_PTR idEvent, DWORD dwTimer) {
	TBBTN *tbb = (TBBTN *) GetWindowLongPtr(hwnd, GWLP_USERDATA);

	InvalidateRect(hwnd, NULL, FALSE);
	if (tbb->bHotLit) {
		if (tbb->trans_state < FADE_SOLID) {
			tbb->trans_state++;
			return;
		}
	} else {
		if (tbb->trans_state > 0) {
			tbb->trans_state--;
			return;
		}
	}

	tbb->bFading = FALSE;
	KillTimer(hwnd, idEvent);
}

LRESULT CALLBACK ToolbarButtonProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	static int timer = 0;
	switch (Message) {
		case WM_CREATE:
		{
			TBBTN *tbb = (TBBTN *) GetWindowLongPtr(hwnd, GWLP_USERDATA);
			tbb->trans_state = 0;
			tbb->bFading = FALSE;
			break;
		}
		case WM_COMMAND:
		{
			switch(wParam)
			{
				case DB_GOTO:
				case DB_MEMPOINT_READ:
				case DB_MEMPOINT_WRITE:
					SendMessage(hwndLastFocus, WM_COMMAND, wParam, 0);
					break;
#ifdef WITH_BACKUPS
				case IDM_05SECOND:
				case IDM_10SECOND:
				case IDM_15SECOND:
				case IDM_20SECOND:
				case IDM_25SECOND:
				case IDM_30SECOND:
				case IDM_35SECOND:
				case IDM_40SECOND:
				case IDM_45SECOND:
				case IDM_50SECOND:
				{
					restore_backup(((int) wParam) - IDM_05SECOND, lpDebuggerCalc);
					SendMessage(GetParent(hwnd), WM_COMMAND, wParam, 0);
					break;
				}
#endif
			}
			break;
		}
		case WM_MOUSEMOVE:
		{
			TBBTN *tbb = (TBBTN *) GetWindowLongPtr(hwnd, GWLP_USERDATA);
			RECT wr;
			GetWindowRect(hwnd, &wr);

			POINT p = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
			ClientToScreen(hwnd, &p);

			if (PtInRect(&wr, p)) {
				if (tbb->bHotLit == FALSE) {
					tbb->bHotLit = TRUE;
					if (tbb->bFading == FALSE) {
						tbb->bFading = TRUE;
						SetTimer(hwnd, timer++, FADE_SPEED, ButtonFadeProc);
					}

					InvalidateRect(hwnd, NULL, TRUE);
					UpdateWindow(hwnd);

					TRACKMOUSEEVENT tme;
					tme.cbSize = sizeof(tme);
					tme.dwFlags = TME_LEAVE;
					tme.hwndTrack = hwnd;
					tme.dwHoverTime = 1;
					TrackMouseEvent(&tme);
				}
			} else {
				tbb->bHotLit = TRUE;
				tbb->MouseState = MOUSE_UP;
				InvalidateRect(hwnd, NULL, TRUE);
				UpdateWindow(hwnd);
			}

			return 0;
		}
		case WM_KEYDOWN:
		{
			TBBTN *tbb = (TBBTN *) GetWindowLongPtr(hwnd, GWLP_USERDATA);

			if (tbb->MouseState != MOUSE_UP)
				return 0;

			switch (wParam) {
				case VK_LEFT:
					if (tbb->prev != NULL)
						SetFocus(tbb->prev->hwnd);
					else {
						TBBTN *last = tbb;
						while (last->next != NULL)
							last = last->next;
						SetFocus(last->hwnd);
					}
					break;
				case VK_RIGHT:
					if (tbb->next != NULL)
						SetFocus(tbb->next->hwnd);
					else {
						TBBTN *last = tbb;
						while (last->prev != NULL)
							last = last->prev;
						SetFocus(last->hwnd);
					}
					break;
				case VK_SPACE:
				case VK_RETURN:
					tbb->MouseState = MOUSE_DOWN;
					InvalidateRect(hwnd, NULL, TRUE);
					break;
			}
			return 0;
		}
		case WM_KEYUP:
		{
			TBBTN *tbb = (TBBTN *) GetWindowLongPtr(hwnd, GWLP_USERDATA);

			switch (wParam) {
				case VK_SPACE:
				case VK_RETURN:
					tbb->MouseState = MOUSE_UP;
					InvalidateRect(hwnd, NULL, TRUE);

					int ID = (int) GetWindowLongPtr(hwnd, GWL_ID);
					SendMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(ID, BN_CLICKED), (LPARAM) hwnd);
					break;
			}
			return 0;
		}
		case WM_SETFOCUS:
		{
			InvalidateRect(hwnd, NULL, TRUE);
			UpdateWindow(hwnd);
			return 0;
		}
		case WM_MOUSELEAVE:
		{
			TBBTN *tbb = (TBBTN *) GetWindowLongPtr(hwnd, GWLP_USERDATA);

			tbb->bHotLit = FALSE;
			if (tbb->bFading == FALSE) {
				tbb->bFading = TRUE;
				SetTimer(hwnd, timer++, FADE_SPEED, ButtonFadeProc);
			}
			InvalidateRect(hwnd, NULL, TRUE);
			UpdateWindow(hwnd);
			return 0;
		}
		case WM_LBUTTONDBLCLK:
		case WM_LBUTTONDOWN:
		{
			TBBTN *tbb = (TBBTN *) GetWindowLongPtr(hwnd, GWLP_USERDATA);

			int xPos = GET_X_LPARAM(lParam);
			RECT rc;
			GetClientRect(hwnd, &rc);
			if (xPos > rc.right - 24 && tbb->bSplitButton) {
				tbb->MouseState = MOUSE_DOWN_SPLIT;
			} else {
				tbb->MouseState = MOUSE_DOWN;
			}
			InvalidateRect(hwnd, NULL, TRUE);
			UpdateWindow(hwnd);
			SetCapture(hwnd);
			SetFocus(hwnd);
			return 0;
		}
		case WM_SIZE:
		{
			TBBTN *tbb = (TBBTN *) GetWindowLongPtr(hwnd, GWLP_USERDATA);

			if (tbb->next == NULL)
				return 0;

			RECT pr;
			GetWindowRect(GetParent(hwnd), &pr);
			RECT rc;
			GetWindowRect(hwnd, &rc);

			HWND hwndChevron;
			TBBTN *chv = tbb->next;
			while (chv->next != NULL)
				chv = chv->next;

			hwndChevron = chv->hwnd;

			TCHAR buf[256];
			GetWindowText(hwnd, buf, sizeof(buf));

			// Did it transect the boundary
			if ((rc.right > pr.right) && (rc.left < pr.right)) {
				// If so, immediately hidden
				ShowWindow(hwnd, SW_HIDE);

				// Is there room for a chevron?
				if (pr.right - rc.left > (4 + 16 + 4)) {
					SetWindowPos(hwndChevron, NULL, rc.left - pr.left, rc.top - pr.top,
							0, 0, SWP_NOSIZE | SWP_NOZORDER);
					ShowWindow(hwndChevron, SW_SHOW);
				}

			// Is it past the boundary?
			} else if (rc.left >= pr.right) {
				ShowWindow(hwnd, SW_HIDE);

			} else if ((tbb->next->hwnd != hwndChevron) && (rc.right + 4 + 4+ 16 + 4) >= pr.right) {
				ShowWindow(hwnd, SW_HIDE);

				SetWindowPos(hwndChevron, NULL, rc.left - pr.left, rc.top - pr.top,
						0, 0, SWP_NOSIZE | SWP_NOZORDER);
				ShowWindow(hwndChevron, SW_SHOW);
			} else {
				ShowWindow(hwnd, SW_SHOW);
				ShowWindow(hwndChevron, SW_HIDE);
			}

			return 0;
		}
		case WM_LBUTTONUP:
		{
			TBBTN *tbb = (TBBTN *) GetWindowLongPtr(hwnd, GWLP_USERDATA);
			tbb->MouseState = MOUSE_UP;
			ReleaseCapture();
			InvalidateRect(hwnd, NULL, TRUE);
			UpdateWindow(hwnd);

			RECT wr, rect;
			GetWindowRect(hwnd, &wr);
			CopyRect(&rect, &wr);
			if (tbb->bSplitButton) {
				wr.right -= 24;
				rect.left = rect.right - 24;
			}

			POINT p = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
			ClientToScreen(hwnd, &p);

			if (PtInRect(&wr, p)) {
				int ID = (int) GetWindowLongPtr(hwnd, GWL_ID);
				SendMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(ID, BN_CLICKED), (LPARAM) hwnd);
			}
			if (PtInRect(&rect, p) && tbb->bSplitButton) {
				HMENU hMenuTrackPopup = GetSubMenu(tbb->hMenu, 0);
				TrackPopupMenu(hMenuTrackPopup,
							TPM_LEFTALIGN | TPM_RIGHTBUTTON,
							wr.left, wr.bottom + 1, 0, hwnd, NULL);
			}
			return 0;
		}
		case WM_SETTEXT:
			{
				SendMessage(hwnd, WM_SETREDRAW, FALSE, NULL);
				DefWindowProc(hwnd, Message, wParam, lParam);
				SendMessage(hwnd, WM_SETREDRAW, TRUE, NULL);
				return 0;
			}
		case WM_PAINT:
		{
			TBBTN *tbb = (TBBTN *) GetWindowLongPtr(hwnd, (int) GWLP_USERDATA);
			HDC hdc;
			PAINTSTRUCT ps;

			RECT rc;
			GetClientRect(hwnd, &rc);
			hdc = BeginPaint(hwnd, &ps);

			HDC hdcBuf = CreateCompatibleDC(hdc);
			HBITMAP hbmBuf = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
			SelectObject(hdcBuf, hbmBuf);

			// Set up the alpha function for the bitmap with alpha values
			BLENDFUNCTION bf;
			bf.BlendOp = AC_SRC_OVER;
			bf.BlendFlags = 0;
			bf.SourceConstantAlpha = 255;
			bf.AlphaFormat = AC_SRC_ALPHA;

			RECT scr;
			GetWindowRect(hwnd, &scr);
			POINT p = { scr.left, scr.top };
			ScreenToClient(GetParent(hwnd), &p);
			OffsetRect(&scr, p.x - scr.left, p.y - scr.top);
			// Fill the background
			PaintToolbarBackground(GetParent(hwnd), hdcBuf, &scr);

			DWORD dwVersion = GetVersion();
			DWORD dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
			BOOL fIsWindows7 = (dwMajorVersion >= 0x06) ? TRUE : FALSE;

			if (tbb->MouseState == MOUSE_DOWN) {
				HDC hdcFrame = CreateCompatibleDC(hdc);
				HBITMAP hbmFrame = LoadBitmap(g_hInst, _T("TBFrameRight"));
				SelectObject(hdcFrame, hbmFrame);

				bf.SourceConstantAlpha = 200;
				AlphaBlend(	hdcBuf, 4, 0, rc.right - 8, rc.bottom,
							hdcFrame, 0, 0, 1, 24,
							bf);

				AlphaBlend( hdcBuf, rc.right - 4, 0, 4, rc.bottom,
							hdcFrame, 0, 0, 4, 24,
							bf);

				HBITMAP hbmFrameLeft = LoadBitmap(g_hInst, _T("TBFrameLeft"));
				SelectObject(hdcFrame, hbmFrameLeft);
				DeleteObject(hbmFrame);

				AlphaBlend(	hdcBuf, 0, 0, 4, rc.bottom,
							hdcFrame, 0, 0, 4, 24,
							bf);

				DeleteObject(hbmFrameLeft);
				if (tbb->bSplitButton)
				{
					HBITMAP hbmFrameMiddle = LoadBitmap(g_hInst, _T("TBFrameMiddle"));
					SelectObject(hdcFrame, hbmFrameMiddle);
					AlphaBlend( hdcBuf, rc.right - 24, 0, 4, rc.bottom,
								hdcFrame, 0, 0, 4, 24,
								bf);
					DeleteObject(hbmFrameMiddle);
				}
				DeleteDC(hdcFrame);
			}
			else if (tbb->MouseState == MOUSE_DOWN_SPLIT) {
				HDC hdcFrame = CreateCompatibleDC(hdc);
				HBITMAP hbmFrame = LoadBitmap(g_hInst, _T("TBFrameRight"));
				SelectObject(hdcFrame, hbmFrame);

				bf.SourceConstantAlpha = 200;
				AlphaBlend(	hdcBuf, rc.right - 24, 0, 20, rc.bottom,
							hdcFrame, 0, 0, 1, 24,
							bf);

				AlphaBlend( hdcBuf, rc.right - 4, 0, 4, rc.bottom,
							hdcFrame, 0, 0, 4, 24,
							bf);
				DeleteObject(hbmFrame);
				HBITMAP hbmFrameMiddle = LoadBitmap(g_hInst, _T("TBFrameMiddle"));
				SelectObject(hdcFrame, hbmFrameMiddle);
				AlphaBlend( hdcBuf, rc.right - 24, 0, 4, rc.bottom,
							hdcFrame, 0, 0, 4, 24,
							bf);
				DeleteObject(hbmFrameMiddle);
				hbmFrame = LoadBitmap(g_hInst, _T("TBHotFrame"));
				SelectObject(hdcFrame, hbmFrame);

				if (hwnd == GetFocus()) {
					bf.SourceConstantAlpha = 100;
				} else {
					bf.SourceConstantAlpha = 100 * tbb->trans_state / FADE_SOLID;
				}
				AlphaBlend(	hdcBuf, 4, 0, rc.right - 8, rc.bottom,
							hdcFrame, 0, 0, 1, 24,
							bf);

				AlphaBlend( hdcBuf, rc.right - 4, 0, 4, rc.bottom,
							hdcFrame, 0, 0, 4, 24,
							bf);

				HBITMAP hbmFrameLeft = LoadBitmap(g_hInst, _T("TBHotFrameLeft"));
				SelectObject(hdcFrame, hbmFrameLeft);
				DeleteObject(hbmFrame);

				AlphaBlend(	hdcBuf, 0, 0, 4, rc.bottom,
							hdcFrame, 0, 0, 4, 24,
							bf);

				DeleteObject(hbmFrameLeft);
				if (tbb->bSplitButton)
				{
					HBITMAP hbmFrameMiddle = LoadBitmap(g_hInst, _T("TBHotFrameMiddle"));
					SelectObject(hdcFrame, hbmFrameMiddle);
					AlphaBlend( hdcBuf, rc.right - 24, 0, 4, rc.bottom,
								hdcFrame, 0, 0, 4, 24,
								bf);
					DeleteObject(hbmFrameMiddle);
				}
				DeleteDC(hdcFrame);
			} else {
				if (tbb->bHotLit || hwnd == GetFocus() || tbb->bFading) {
					HDC hdcFrame = CreateCompatibleDC(hdc);
					HBITMAP hbmFrame = LoadBitmap(g_hInst, _T("TBHotFrame"));
					SelectObject(hdcFrame, hbmFrame);

					if (hwnd == GetFocus()) {
						bf.SourceConstantAlpha = 100;
					} else {
						bf.SourceConstantAlpha = 100 * tbb->trans_state / FADE_SOLID;
					}
					AlphaBlend(	hdcBuf, 4, 0, rc.right - 8, rc.bottom,
								hdcFrame, 0, 0, 1, 24,
								bf);

					AlphaBlend( hdcBuf, rc.right - 4, 0, 4, rc.bottom,
								hdcFrame, 0, 0, 4, 24,
								bf);

					HBITMAP hbmFrameLeft = LoadBitmap(g_hInst, _T("TBHotFrameLeft"));
					SelectObject(hdcFrame, hbmFrameLeft);
					DeleteObject(hbmFrame);

					AlphaBlend(	hdcBuf, 0, 0, 4, rc.bottom,
								hdcFrame, 0, 0, 4, 24,
								bf);

					DeleteObject(hbmFrameLeft);
					if (tbb->bSplitButton)
					{
						HBITMAP hbmFrameMiddle = LoadBitmap(g_hInst, _T("TBHotFrameMiddle"));
						SelectObject(hdcFrame, hbmFrameMiddle);
						AlphaBlend( hdcBuf, rc.right - 24, 0, 4, rc.bottom,
									hdcFrame, 0, 0, 4, 24,
									bf);
						DeleteObject(hbmFrameMiddle);
					}
					DeleteDC(hdcFrame);
				}
				if (tbb->bHotLit || tbb->bFading) {
					// Create the header for the bitmap with alpha values
					BITMAPINFO bmi;
					ZeroMemory(&bmi, sizeof(BITMAPINFO));

					BITMAPINFOHEADER *bi = &bmi.bmiHeader;
					bi->biSize = sizeof(BITMAPINFOHEADER);
					bi->biWidth = 1;
					bi->biHeight = rc.bottom - 4;
					bi->biPlanes = 1;
					bi->biBitCount = 32;
					bi->biCompression = BI_RGB;

					int width = bi->biWidth;
					int height = bi->biHeight;

					HDC hdcGrad = CreateCompatibleDC(hdcBuf);
					// Create a solid brush of the gradient color
					SelectObject(hdcGrad, GetStockObject(WHITE_BRUSH));

					SelectObject(hdcGrad, GetStockObject(DC_PEN));
					SetDCPenColor(hdcGrad, RGB(255, 255, 255));

					BYTE *pBits;
					HBITMAP hbmGrad = CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (void **) &pBits, NULL, 0);

					SelectObject(hdcGrad, hbmGrad);
					Rectangle(hdcGrad, 0, 0, width, height);

					int y;

					BYTE * pPixel = pBits;
					for (y = 0; y < height; y++, pPixel+=4) {
						pPixel[3] = 255*(y+1)/height/3;

						pPixel[0] = pPixel[0] * pPixel[3] / 0xFF;
						pPixel[1] = pPixel[1] * pPixel[3] / 0xFF;
						pPixel[2] = pPixel[2] * pPixel[3] / 0xFF;

					}

					bf.SourceConstantAlpha = 100 * tbb->trans_state / FADE_SOLID;
					AlphaBlend(	hdcBuf, 2, 2, rc.right - 4, rc.bottom - 4,
								hdcGrad, 0, 0, 1, height,
								bf);

					DeleteObject(hbmGrad);
					DeleteDC(hdcGrad);
				}
			}

			bf.SourceConstantAlpha = 255;

			int ox = 0, oy = 0;
			if (tbb->MouseState == MOUSE_DOWN || tbb->MouseState == MOUSE_DOWN_SPLIT)
				ox = oy = 1;

			if (tbb->bSplitButton)
			{
				HDC hdcDownBtn = CreateCompatibleDC(hdc);
				HBITMAP hbmDownArrow;
				if (fIsWindows7 == TRUE)
					hbmDownArrow = LoadBitmap(g_hInst, _T("TBDownArrow7"));
				else
					hbmDownArrow = LoadBitmap(g_hInst, _T("TBDownArrow"));
				SelectObject(hdcDownBtn, hbmDownArrow);
				AlphaBlend(	hdcBuf, rc.right - 20 + ox, 3 + oy, 16, 16,
							hdcDownBtn, 0, 0, 16, 16,
							bf);
				DeleteBitmap(hbmDownArrow);
				DeleteDC(hdcDownBtn);
			}

			if (tbb->MouseState == MOUSE_DOWN_SPLIT)
				ox = oy = 0;

			HDC hdcBtn = CreateCompatibleDC(hdc);
			HBITMAP hbmPrior = (HBITMAP) SelectObject(hdcBtn, tbb->hbmIcon);
			AlphaBlend(	hdcBuf, 4 + ox, 4 + oy, 16, 16,
						hdcBtn, 0, 0, 16, 16,
						bf);

			SetBkMode(hdcBuf, TRANSPARENT);

			SelectObject(hdcBuf, hfontSegoe);

			TCHAR szTitle[32];
			GetWindowText(hwnd, szTitle, 32);

			RECT r = {16 + 9 + ox, 2 + oy, 400 + ox, 24 + oy};
			if (fIsWindows7 == TRUE)
				SetTextColor(hdcBuf, RGB(30, 57, 91));
			else
				SetTextColor(hdcBuf, RGB(0, 0, 0));
			DrawText(hdcBuf, szTitle, -1, &r, DT_LEFT | DT_SINGLELINE | DT_VCENTER);

			if (fIsWindows7 == FALSE) {
				OffsetRect(&r, -1, -1);
				SetTextColor(hdcBuf, RGB(255, 255, 255));
				DrawText(hdcBuf, szTitle, -1, &r, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
			}

			SelectObject(hdcBtn, hbmPrior);
			DeleteDC(hdcBtn);

			BitBlt(hdc, 0, 0, rc.right, rc.bottom, hdcBuf, 0, 0, SRCCOPY);

			DeleteObject(hbmBuf);
			DeleteDC(hdcBuf);

			EndPaint(hwnd, &ps);
			return 0;
		}
		case WM_CLOSE:
			DestroyWindow(hwnd);
			return 0;
		case WM_DESTROY: {
			TBBTN *tbb = (TBBTN *) GetWindowLongPtr(hwnd, GWLP_USERDATA);
			DeleteObject(tbb->hbmIcon);
			free(tbb);
			return 0;
		}
		default:
			return CallWindowProc(OldButtonProc, hwnd, Message, wParam, lParam);
	}
	return CallWindowProc(OldButtonProc, hwnd, Message, wParam, lParam);
}

static TBBTN *prevBtn = NULL;
int CreateToolbarButton(HWND hwndParent, TCHAR *szCaption, TCHAR *szTooltip, TCHAR *szIcon, int x, int y, int ID, BOOL splitButton, HMENU hMenu = NULL) {
	static HWND hwndTip = NULL;

	TBBTN *tbb = (TBBTN *) malloc(sizeof(TBBTN));
	memset(tbb, 0, sizeof(TBBTN));

	tbb->hbmIcon = LoadBitmap(g_hInst, szIcon);
	tbb->bHotLit = FALSE;
	tbb->MouseState = MOUSE_UP;
	tbb->bSplitButton = splitButton;
	tbb->prev = prevBtn;
	if (prevBtn)
		prevBtn->next = tbb;
	tbb->next = NULL;

	HWND hwndBtn = CreateWindow(
		_T("BUTTON"),
		szCaption,
		WS_CHILD | BS_PUSHBUTTON,
		x, y, 60, 24,
		hwndParent, (HMENU) ID, g_hInst, NULL);
	SetWindowFont(hwndBtn, hfontSegoe, FALSE);

	tbb->hwnd = hwndBtn;
	prevBtn = tbb;

	HDC hdc = GetDC(hwndBtn);
	RECT r = {0, 0, 0, 0};

	SelectObject(hdc, hfontSegoe);
	DrawText(hdc, szCaption, (int) _tcslen(szCaption), &r, DT_CALCRECT);

	ReleaseDC(hwndBtn, hdc);

	int img_sz_seperator = 8;
	if (_tcslen(szCaption) == 0)
		img_sz_seperator = 0;
	int splitSize = 0;
	if (tbb->bSplitButton) {
		splitSize = 20;
		tbb->hMenu = hMenu;
	}
	MoveWindow(hwndBtn, x, y, 4 + 16 + img_sz_seperator + splitSize + r.right + 4, 24, FALSE);
	ShowWindow(hwndBtn, SW_SHOW);

	OldButtonProc = (WNDPROC) GetWindowLongPtr(hwndBtn, GWLP_WNDPROC);
	SetWindowLongPtr(hwndBtn, GWLP_WNDPROC, (LONG_PTR) ToolbarButtonProc);
	SetWindowLongPtr(hwndBtn, GWLP_USERDATA, (LONG_PTR) tbb);

	if (hwndTip == NULL) {
		hwndTip = CreateWindowEx(
				(DWORD)NULL,
				TOOLTIPS_CLASS,
				NULL, WS_POPUP | TTS_ALWAYSTIP,
				CW_USEDEFAULT, CW_USEDEFAULT,
				CW_USEDEFAULT, CW_USEDEFAULT,
				hwndParent, NULL, g_hInst, NULL);

		SetWindowPos(hwndTip, HWND_TOPMOST,0, 0, 0, 0,
					 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
		SendMessage(hwndTip, TTM_ACTIVATE, TRUE, 0);
	}

	TOOLINFO toolInfo = {0};
	toolInfo.cbSize = sizeof(toolInfo);
	toolInfo.hwnd = hwndParent;
	toolInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
	toolInfo.uId = (UINT_PTR) hwndBtn;
	toolInfo.lpszText = szTooltip;
	SendMessage(hwndTip, TTM_ADDTOOL, 0, (LPARAM) &toolInfo);

	return x + 4 + 16 + 8 + r.right + 4 + 4 + splitSize;
}

BOOL CALLBACK EnumToolbarRedraw(HWND hwndChild, LPARAM lParam) {
	SendMessage(hwndChild, WM_SIZE, 0, 0);

	InvalidateRect(hwndChild, NULL, FALSE);
	UpdateWindow(hwndChild);
	return TRUE;
}

waddr_t z80_to_waddr(uint16_t addr) {
	bank_t *pb = &lpDebuggerCalc->mem_c.banks[mc_bank(addr)];

	waddr_t waddr;
	waddr.is_ram = pb->ram;
	waddr.page = pb->page;
	waddr.addr = mc_base(addr);
	return waddr;
}

static HMENU rewindmenu;
HMENU CreateRewindMenu() {
	TCHAR buf[256];
	int i;
	rewindmenu = LoadMenu(g_hInst, MAKEINTRESOURCE(IDR_DISASM_REWIND_MENU));
#if WITH_BACKUPS
	float j = 1 / ((float) num_backup_per_sec);
	for (i = 0; i < MAX_BACKUPS; i++) {
		StringCbPrintf(buf, sizeof(buf), _T("%.2f"), j * (i + 1));
		StringCbCat(buf, sizeof(buf), _T(" seconds"));
		ModifyMenu(rewindmenu, IDM_05SECOND + i, MF_BYCOMMAND | MF_STRING, 0, buf);
	}
#endif
	return rewindmenu;
}

void ChangeRunButtonIconAndText(TBBTN *tbb) {
	if (tbb->hbmIcon)
		DeleteObject(tbb->hbmIcon);
	if (lpDebuggerCalc->running) {
		tbb->hbmIcon = LoadBitmap(g_hInst, _T("DBStop"));
		Edit_SetText(tbb->hwnd, _T("Stop"));
	} else {
		tbb->hbmIcon = LoadBitmap(g_hInst, _T("DBRun"));
		Edit_SetText(tbb->hwnd, _T("Run"));
	}
	RECT rc;
	GetClientRect(tbb->hwnd, &rc);
	InvalidateRect(tbb->hwnd, &rc, TRUE);
}

LRESULT CALLBACK ToolBarProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {

	switch (Message) {
		case WM_CREATE: {

			prevBtn = NULL;
			SelectObject(GetDC(hwnd), hfontSegoe);
			int next = 4;
			next = CreateToolbarButton(hwnd, _T("Run"), _T("Run the calculator."), _T("DBRun"), next, 4, 999, FALSE);
			next = CreateToolbarButton(hwnd, _T("Toggle Breakpoint"), _T("Toggle the breakpoint on the current selection."), _T("DBBreak"), next, 4, 1000, FALSE);
			HMENU hmenu = LoadMenu(g_hInst, (LPCTSTR) IDR_DISASM_WATCH_MENU);
			next = CreateToolbarButton(hwnd, _T("Toggle Watchpoint"), _T("Toggle a memory breakpoint at the current selection."), _T("DBMemBreak"), next, 4, 1001, TRUE, hmenu);
			next = CreateToolbarButton(hwnd, _T("Step"), _T("Run a single command."), _T("DBStep"), next, 4, 1002, FALSE);
			next = CreateToolbarButton(hwnd, _T("Step Over"), _T("Run a single line."), _T("DBStepOver"), next, 4, 1003, FALSE);
			next = CreateToolbarButton(hwnd, _T("Goto"), _T("Goto an address in RAM or Flash."), _T("DBGoto"), next, 4, 1004, FALSE);
			/*hmenu = CreateRewindMenu();
			next = CreateToolbarButton(hwnd, _T("Rewind"), _T("Restores to a previous state."), NULL, next, 4, 1005, TRUE, hmenu);*/
			//next = CreateToolbarButton(hwnd, "Track", "Create a variable track window.", "DBGoto", next, 4, 1005);
			TCHAR *szChevronBMP;
			DWORD dwVersion = GetVersion();
			DWORD dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
			if (dwMajorVersion < 0x06)
				szChevronBMP = _T("CHEVRON");
			else
				szChevronBMP = _T("CHEVRON7");
			next = CreateToolbarButton(hwnd, _T(""), _T("Display additional commands."), szChevronBMP, next, 4, 1006, FALSE);

			return 0;
		}
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case DB_BREAKPOINT:
				case DB_MEMPOINT_READ:
				case DB_MEMPOINT_WRITE:
					SendMessage(hwndLastFocus, WM_COMMAND, wParam, 0);
					break;
#ifdef WITH_BACKUPS
				case IDM_05SECOND:
				case IDM_10SECOND:
				case IDM_15SECOND:
				case IDM_20SECOND:
				case IDM_25SECOND:
				case IDM_30SECOND:
				case IDM_35SECOND:
				case IDM_40SECOND:
				case IDM_45SECOND:
				case IDM_50SECOND:
				{
					restore_backup(((int) wParam) - IDM_05SECOND, lpDebuggerCalc);
					SendMessage(GetParent(hwnd), WM_COMMAND, wParam, 0);
					break;
				}
#endif
				case 999: {
					TBBTN *tbb = (TBBTN *) GetWindowLongPtr((HWND) lParam, GWLP_USERDATA);
					if (lpDebuggerCalc->running)
						SendMessage(GetParent(hwnd), WM_COMMAND, DB_STOP, 0);
					else
						SendMessage(GetParent(hwnd), WM_COMMAND, DB_RUN, 0);
					ChangeRunButtonIconAndText(tbb);
					break;
				}
				case 1000:
					SendMessage(hwndLastFocus, WM_COMMAND, DB_BREAKPOINT, 0);
					break;
				case 1001:
					SendMessage(hwndLastFocus, WM_COMMAND, DB_MEMPOINT_WRITE, 0);
					break;
				case 1002:
					SendMessage(GetParent(hwnd), WM_COMMAND, DB_STEP, 0);
					break;
				case 1003:
					SendMessage(GetParent(hwnd), WM_COMMAND, DB_STEPOVER, 0);
					break;
				case 1004:
					SendMessage(hwndLastFocus, WM_COMMAND, DB_GOTO, 0);
					break;
				case 1006:
				{
					HMENU hmenu = CreatePopupMenu();
					MENUITEMINFO mii;
					BITMAPINFO biMenu;

					biMenu.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
					biMenu.bmiHeader.biPlanes = 1;
					biMenu.bmiHeader.biCompression = BI_RGB;
					biMenu.bmiHeader.biWidth = 16;
					biMenu.bmiHeader.biHeight = 16;
					biMenu.bmiHeader.biBitCount = 32;
					biMenu.bmiHeader.biSizeImage = 0;
					biMenu.bmiHeader.biXPelsPerMeter = 0;
					biMenu.bmiHeader.biYPelsPerMeter = 0;
					biMenu.bmiHeader.biClrUsed = 0;
					biMenu.bmiHeader.biClrImportant = 0;

					void **ppvBits;
					CreateDIBSection(GetDC(hwnd), &biMenu, DIB_RGB_COLORS, (void **) &ppvBits, NULL, 0);

					printf("context menu\n");

					TCHAR WindowText[256];

					mii.cbSize = sizeof(MENUITEMINFO);
					mii.fMask = MIIM_STATE | MIIM_ID | MIIM_STRING | MIIM_BITMAP;
					mii.fType = MFT_STRING;
					mii.fState = MFS_ENABLED;
					mii.hSubMenu = NULL;
					mii.hbmpChecked = NULL;
					mii.hbmpUnchecked = NULL;
					mii.dwItemData = 0;
					mii.dwTypeData = WindowText;

					TBBTN *tbb = (TBBTN *) GetWindowLongPtr((HWND) lParam, GWLP_USERDATA);
					//tbb->bMouseDown = TRUE;
					tbb = tbb->prev;

					BITMAPINFO *pbmi = (BITMAPINFO *) malloc(sizeof(BITMAPINFO));
					pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
					pbmi->bmiHeader.biWidth = 16;
					pbmi->bmiHeader.biHeight = 16;
					pbmi->bmiHeader.biPlanes = 1;
					pbmi->bmiHeader.biBitCount = 32;
					pbmi->bmiHeader.biCompression = BI_RGB;
					pbmi->bmiHeader.biSizeImage = 0;
					pbmi->bmiHeader.biXPelsPerMeter = 90;
					pbmi->bmiHeader.biYPelsPerMeter = 90;
					pbmi->bmiHeader.biClrUsed = 0;
					pbmi->bmiHeader.biClrImportant = 0;

					void *pvBits;


					HDC hdcTemp = CreateCompatibleDC(GetDC(hwnd));

					//DeleteDC(hdcTemp);

					int i;
					for (i = 0; tbb != NULL && !IsWindowVisible(tbb->hwnd); i++, tbb = tbb->prev) {
						if (tbb->bSplitButton)
						{
							GetWindowText(tbb->hwnd, WindowText, sizeof(WindowText));
							HMENU hMenuToAdd = GetSubMenu(tbb->hMenu, 0);
							InsertMenu(hmenu, mii.wID, MF_POPUP | MF_STRING| MF_BYPOSITION, (UINT) hMenuToAdd, WindowText);
						} else {
							mii.cch = GetWindowText(tbb->hwnd, WindowText, sizeof(WindowText));
							mii.wID = 1005-i;

							HBITMAP hbmDIB = CreateDIBSection(
									GetDC(hwnd),
									pbmi,
									DIB_RGB_COLORS,
									&pvBits,
									NULL,
									0
							);

							if (hbmDIB == NULL) {
								MessageBox(NULL, _T("fuck"), _T("fuck"), MB_OK);
								break;
							}
							HBITMAP hbmBackup = (HBITMAP) SelectObject(hdcTemp, hbmDIB);

							HDC hdcIcon = CreateCompatibleDC(GetDC(hwnd));
							HBITMAP hbmBackup2 = (HBITMAP) SelectObject(hdcIcon, tbb->hbmIcon);

							BLENDFUNCTION bf;
							bf.BlendOp = AC_SRC_OVER;
							bf.BlendFlags = 0;
							bf.SourceConstantAlpha = 255;
							bf.AlphaFormat = AC_SRC_ALPHA;

							BitBlt(	hdcTemp, 0, 0, 16, 16,
										hdcIcon, 0, 0,
										SRCCOPY);

							SelectObject(hdcIcon, hbmBackup2);
							DeleteDC(hdcIcon);

							SelectObject(hdcTemp, hbmBackup);

							mii.hbmpItem = hbmDIB;

							if (InsertMenuItem(hmenu, 0, TRUE, &mii) == FALSE) {
								_tprintf_s(_T("Failed to insert\n"));
							}
						}
					}

					RECT r;
					GetWindowRect((HWND) lParam, &r);

					OnContextMenu(hwnd, r.left, r.bottom, hmenu);
					break;
				}
			}
			return 0;
		case WM_NOTIFY: {
			return 0;
		}
		case WM_PAINT: {
			RECT rc;
			GetClientRect(hwnd, &rc);

			DWORD dwVersion = GetVersion();
			DWORD dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
			BOOL fIsWindows7 = (dwMajorVersion >= 0x06) ? TRUE : FALSE;

			HDC hdc;
			PAINTSTRUCT ps;

			hdc = BeginPaint(hwnd, &ps);

			HDC hdcBuf = CreateCompatibleDC(hdc);
			HBITMAP hbmBuf = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
			SelectObject(hdcBuf, hbmBuf);

			if (IsAppThemed()) {
				HDC hdcRight = CreateCompatibleDC(hdcBuf);
				HBITMAP hbmRight = LoadBitmap(g_hInst, fIsWindows7 == TRUE ? _T("TBRIGHT7") : _T("TBRIGHT"));
				SelectObject(hdcRight, hbmRight);

				if (rc.right > 3 + 120) {
					StretchBlt(hdcBuf, 3, 0, rc.right - 123, 32, hdcRight, 0, 0, 1, 32, SRCCOPY);
				}

				BitBlt(hdcBuf, rc.right - 120, 0, 120, 32, hdcRight, 0, 0, SRCCOPY);

				DeleteObject(hbmRight);
				DeleteDC(hdcRight);

				HDC hdcLeft = CreateCompatibleDC(hdcBuf);
				HBITMAP hbmLeft = LoadBitmap(g_hInst, fIsWindows7 == TRUE ? _T("TBLEFT7") : _T("TBLEFT"));
				SelectObject(hdcLeft, hbmLeft);

				BitBlt(hdcBuf, 0, 0, 3, 32, hdcLeft, 0, 0, SRCCOPY);

				DeleteObject(hbmLeft);
				DeleteDC(hdcLeft);
			} else {
				FillRect(hdcBuf, &rc, GetSysColorBrush(COLOR_MENU));
			}

			// Set up the alpha function for the bitmap with alpha values
			BLENDFUNCTION bf;
			bf.BlendOp = AC_SRC_OVER;
			bf.BlendFlags = 0;
			bf.SourceConstantAlpha = 255;
			bf.AlphaFormat = AC_SRC_ALPHA;

			// Create the header for the bitmap with alpha values
			BITMAPINFO bmi;
			ZeroMemory(&bmi, sizeof(BITMAPINFO));

			BITMAPINFOHEADER *bi = &bmi.bmiHeader;
			bi->biSize = sizeof(BITMAPINFOHEADER);
			bi->biWidth = rc.right;
			bi->biHeight = 1;
			bi->biPlanes = 1;
			bi->biBitCount = 32;
			bi->biCompression = BI_RGB;

			int width = bi->biWidth;
			int height = bi->biHeight;

			if (fIsWindows7 == FALSE)
			{
				HDC hdcGrad = CreateCompatibleDC(hdc);
				// Create a solid brush of the gradient color
				HBRUSH hbrGrad = CreateSolidBrush(RGB(0, 190, 0));
				SelectObject(hdcGrad, hbrGrad);

				SelectObject(hdcGrad, GetStockObject(DC_PEN));
				SetDCPenColor(hdcGrad, RGB(0, 190, 0));

				BYTE *pBits;
				HBITMAP hbmGrad = CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (void**) &pBits, NULL, 0);

				SelectObject(hdcGrad, hbmGrad);

				// Fill it with green
				Rectangle(hdcGrad, 0, 0, width, height);

				int x;

				BYTE * pPixel = pBits;
				for (x = 0; x < width; x++, pPixel+=4) {
					pPixel[3] = 255*(x+1)/width/8;

					pPixel[0] = pPixel[0] * pPixel[3] / 0xFF;
					pPixel[1] = pPixel[1] * pPixel[3] / 0xFF;
					pPixel[2] = pPixel[2] * pPixel[3] / 0xFF;

				}

				AlphaBlend(	hdcBuf, 0, 0, rc.right, rc.bottom,
							hdcGrad, 0, 0, width, 1,
							bf);

				DeleteObject(hbmGrad);
				DeleteObject(hbrGrad);
				DeleteDC(hdcGrad);
			}

			BitBlt(hdc, 0, 0, rc.right, rc.bottom, hdcBuf, 0, 0, SRCCOPY);

			DeleteObject(hbmBuf);
			DeleteDC(hdcBuf);

			EndPaint(hwnd, &ps);
			break;
		}
		case WM_ERASEBKGND:
			return 0;
		case WM_SIZE:
			InvalidateRect(hwnd, NULL, FALSE);
			UpdateWindow(hwnd);
			EnumChildWindows(hwnd, EnumToolbarRedraw, 0);
			return 0;
		case WM_USER:
			InvalidateRect(hwnd, NULL, FALSE);
			return 0;
		default:
			return DefWindowProc(hwnd, Message, wParam, lParam);

	}
	return 0;
}
