#include "stdafx.h"

#include "gui.h"
#include "guisize.h"
#include "calc.h"
#include "guibuttons.h"
#include "guidetached.h"

extern HINSTANCE g_hInst;

LRESULT CALLBACK DetachedProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	static LPCALC lpCalc;
	switch (msg) {
	case WM_CREATE: {
		lpCalc = (LPCALC)((LPCREATESTRUCT)lParam)->lpCreateParams;

		int lcdWidth =  lpCalc->cpu.pio.lcd->display_width;
		int lcdHeight = lpCalc->cpu.pio.lcd->height;
		lpCalc->hwndDetachedLCD = CreateWindowEx(
			0,
			g_szLCDName,
			_T("LCD"),
			WS_VISIBLE | WS_CHILD,
			0, 0, lcdWidth * lpCalc->scale, lcdHeight * lpCalc->scale,
			hwnd, NULL, g_hInst, (LPVOID *)lpCalc);
		return FALSE;
	}
		case WM_DESTROY:
			lpCalc->hwndDetachedLCD = NULL;
			lpCalc->hwndDetachedFrame = NULL;
			return FALSE;
		case WM_KEYDOWN:
			HandleKeyDown(lpCalc, wParam);
			return FALSE;
		case WM_KEYUP:
			if (wParam) {
				HandleKeyUp(lpCalc, wParam);
			}
			return FALSE;
		case WM_COMMAND:
			return FALSE;
		case WM_SIZING: {
			return HandleSizingMessage(hwnd, lpCalc, wParam, (RECT *)lParam);
		}
		case WM_SIZE: {
			return HandleSizeMessage(hwnd, lpCalc->hwndDetachedLCD, lpCalc, FALSE);
		}
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}