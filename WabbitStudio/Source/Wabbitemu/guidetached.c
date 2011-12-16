#include "stdafx.h"

#include "gui.h"
#include "calc.h"
#include "guidetached.h"

extern HINSTANCE g_hInst;

LRESULT CALLBACK DetachedProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	static LPCALC lpCalc;
	switch (msg) {
		case WM_CREATE:
			lpCalc = (LPCALC) ((LPCREATESTRUCT) lParam)->lpCreateParams;
			lpCalc->hwndDetachedLCD = CreateWindowEx(
			0,
			g_szLCDName,
			_T("LCD"),
			WS_VISIBLE | WS_CHILD,
			0, 0, (lpCalc->rectLCD.right - lpCalc->rectLCD.left) * lpCalc->scale, 64 * lpCalc->scale,
			hwnd, NULL, g_hInst,  (LPVOID *) lpCalc);
			return FALSE;
		case WM_DESTROY:
			lpCalc->hwndDetachedLCD = NULL;
			lpCalc->hwndDetachedFrame = NULL;
			return FALSE;
		case WM_COMMAND:
			return FALSE;
		case WM_SIZING: {
			RECT *prc = (RECT *) lParam;
			LONG ClientAdjustWidth, ClientAdjustHeight;
			LONG AdjustWidth, AdjustHeight;
			int width = (lpCalc->rectLCD.right - lpCalc->rectLCD.left) / 2;

			// Adjust for border and menu
			RECT rc = {0, 0, 0, 0};
			AdjustWindowRect(&rc, WS_TILEDWINDOW, FALSE);

			ClientAdjustWidth = rc.right - rc.left;
			ClientAdjustHeight = rc.bottom - rc.top;


			switch (wParam) {
				case WMSZ_BOTTOMLEFT:
				case WMSZ_LEFT:
				case WMSZ_TOPLEFT:
					prc->left -= width / 4;
					break;
				default:
					prc->right += width / 4;
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
			AdjustWidth = (prc->right - prc->left - ClientAdjustWidth) % width;
			AdjustHeight = (prc->bottom - prc->top - ClientAdjustHeight) % 64;

			int cx_mult = (prc->right - prc->left - ClientAdjustWidth) / width;
			int cy_mult = (prc->bottom - prc->top - ClientAdjustHeight) / 64;

			while (cx_mult < 2 || cy_mult < 2) {
				if (cx_mult < 2) {cx_mult++; AdjustWidth -= width;}
				if (cy_mult < 2) {cy_mult++; AdjustHeight -= 64;}
			}

			if (cx_mult > cy_mult)
				AdjustWidth += (cx_mult - cy_mult) * width;
			else if (cy_mult > cx_mult)
				AdjustHeight += (cy_mult - cx_mult) * 64;


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

			return TRUE;
		}
		case WM_SIZE: {
			RECT screen;

			int width = lpCalc->rectLCD.right - lpCalc->rectLCD.left;
			SetRect(&screen,
				0, 0,
				(int) (width / 2 * lpCalc->scale),
				(int) (64 * lpCalc->scale));

			MoveWindow(lpCalc->hwndDetachedLCD, screen.left, screen.top,
				screen.right-screen.left, screen.bottom-screen.top, FALSE);
			ValidateRect(hwnd, &screen);

			UpdateWindow(lpCalc->hwndDetachedLCD);
			return 0;
		}
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}