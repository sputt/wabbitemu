#include "stdafx.h"

#include "gui.h"
#include "guisize.h"
#include "calc.h"
#include "guibuttons.h"
#include "guidetached.h"

extern HINSTANCE g_hInst;

LRESULT CALLBACK DetachedProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_CREATE: {
		LPCALC lpCalc = (LPCALC)((LPCREATESTRUCT)lParam)->lpCreateParams;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)lpCalc);

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
	case WM_DESTROY: {
		LPCALC lpCalc = (LPCALC)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		lpCalc->hwndDetachedLCD = NULL;
		lpCalc->hwndDetachedFrame = NULL;
		return FALSE;
	}
	case WM_KEYDOWN: {
		LPCALC lpCalc = (LPCALC)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		HandleKeyDown(lpCalc, wParam);
		return FALSE;
	}
	case WM_KEYUP: {
		if (wParam) {
			LPCALC lpCalc = (LPCALC)GetWindowLongPtr(hwnd, GWLP_USERDATA);
			HandleKeyUp(lpCalc, wParam);
		}
		return FALSE;
	}
	case WM_COMMAND:
		return FALSE;
	case WM_SIZING: {
		LPCALC lpCalc = (LPCALC)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		LCDBase_t *lcd = lpCalc->cpu.pio.lcd;
		return HandleLCDSizingMessage(hwnd, NULL, lpCalc, wParam, (RECT *)lParam, lcd->display_width);
	}
	case WM_SIZE: {
		LPCALC lpCalc = (LPCALC)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		return HandleSizeMessage(hwnd, lpCalc->hwndDetachedLCD, lpCalc, FALSE);
	}
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}