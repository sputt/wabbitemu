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
		LPMAINWINDOW lpMainWindow = (LPMAINWINDOW)((LPCREATESTRUCT)lParam)->lpCreateParams;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)lpMainWindow);
		LPCALC lpCalc = lpMainWindow->lpCalc;

		int lcdWidth =  lpCalc->cpu.pio.lcd->display_width;
		int lcdHeight = lpCalc->cpu.pio.lcd->height;
		lpCalc->hwndDetachedLCD = CreateWindowEx(
			0,
			g_szLCDName,
			_T("LCD"),
			WS_VISIBLE | WS_CHILD,
			0, 0, lcdWidth * lpCalc->scale, lcdHeight * lpCalc->scale,
			hwnd, NULL, g_hInst, (LPVOID *)lpMainWindow);
		return FALSE;
	}
	case WM_DESTROY: {
		LPMAINWINDOW lpMainWindow = (LPMAINWINDOW)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		if (lpMainWindow == NULL) {
			break;
		}

		LPCALC lpCalc = lpMainWindow->lpCalc;
		lpCalc->hwndDetachedLCD = NULL;
		lpCalc->hwndDetachedFrame = NULL;
		return FALSE;
	}
	case WM_KEYDOWN: {
		LPMAINWINDOW lpMainWindow = (LPMAINWINDOW)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		if (lpMainWindow == NULL) {
			break;
		}

		LPCALC lpCalc = lpMainWindow->lpCalc;
		HandleKeyDown(lpMainWindow, lpCalc, wParam);
		return FALSE;
	}
	case WM_KEYUP: {
		if (wParam == 0) {
			break;
		}

		LPMAINWINDOW lpMainWindow = (LPMAINWINDOW)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		if (lpMainWindow == NULL) {
			break;
		}

		LPCALC lpCalc = lpMainWindow->lpCalc;
		HandleKeyUp(lpMainWindow, lpCalc, wParam);
		return FALSE;
	}
	case WM_COMMAND:
		return FALSE;
	case WM_SIZING: {
		LPMAINWINDOW lpMainWindow = (LPMAINWINDOW)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		if (lpMainWindow == NULL) {
			break;
		}

		LPCALC lpCalc = lpMainWindow->lpCalc;
		LCDBase_t *lcd = lpCalc->cpu.pio.lcd;
		return HandleLCDSizingMessage(hwnd, NULL, lpCalc, wParam, (RECT *)lParam, lcd->display_width);
	}
	case WM_SIZE: {
		LPMAINWINDOW lpMainWindow = (LPMAINWINDOW)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		if (lpMainWindow == NULL) {
			break;
		}

		LPCALC lpCalc = lpMainWindow->lpCalc;
		return HandleSizeMessage(hwnd, lpCalc->hwndDetachedLCD, lpCalc, FALSE);
	}
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}