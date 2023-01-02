#include "stdafx.h"

#include "guiteacherview.h"
#include "gui.h"
#include "savestate.h"

#define APPBACKUPSCREEN 0x9872
#define CX_CURRENT_APP 0x859A

#define YEQU_CONTEXT 0x49
#define STAT_PLOT_CONTEXT 0x55
#define WINDOW_CONTEXT 0x48
#define TABLE_CONTEXT 0x4A
#define TABLESET_CONTEXT 0x4B
#define LIST_CONTEXT 0x43

#define GRAPH_FLAG 0x89F3

extern HINSTANCE g_hInst;

void HandleTeacherViewKey(HWND hwndTeacherView, LPCALC lpCalc) {
	uint8_t graph_flag = mem_read(&lpCalc->mem_c, (uint16_t)GRAPH_FLAG);

	// this flag indicates we need to redraw the graph
	if (graph_flag & BIT(0)) {
		SendMessage(hwndTeacherView, WM_USER, 0, 0);
		mem_write(&lpCalc->mem_c, (uint16_t)GRAPH_FLAG, BIT(0));
	}
}

static void DrawScreen(Graphics &graphics, int x, int y, int width, int height, LPCALC lpCalc) {
	LCDBase_t *lcd = lpCalc->cpu.pio.lcd;
	uint8_t *screen = lcd->image(lcd);
	LPBITMAPINFO info = GetLCDColorPalette(lpCalc->model, lcd);
	Bitmap bitmap(info, screen);

	Rect destRect(x, y, width, height);
	graphics.DrawImage(&bitmap, destRect, 0, 0, lcd->display_width, lcd->height, UnitPixel);
	Pen pen((ARGB)Color::Black);
	graphics.DrawRectangle(&pen, destRect);
}

static void ExecuteCustomCode(LPCALC lpNewCalc) {
	lpNewCalc->cpu.halt = FALSE;
	lpNewCalc->cpu.pc = APPBACKUPSCREEN;

	while (lpNewCalc->cpu.halt != TRUE) {
		CPU_step(&lpNewCalc->cpu);
	}
}

static void ExecuteContextSwitch(LPCALC lpNewCalc, uint8_t newContext) {
	uint8_t code[13] = { 0x3E, newContext, 0xEF, 0x33, 0x40,
		0x3E, 0x10, 0xFD, 0x77, 0x0C, 0xEF, 0x1E, 0x40 };
	for (int i = 0; i < ARRAYSIZE(code); i++) {
		mem_write(&lpNewCalc->mem_c, (uint16_t)(APPBACKUPSCREEN + i), code[i]);
	}

	ExecuteCustomCode(lpNewCalc);
}

static void GotoYEquScreen(LPCALC lpNewCalc) {
	ExecuteContextSwitch(lpNewCalc, YEQU_CONTEXT);
}

static void GotoStatPlotScreen(LPCALC lpNewCalc) {
	ExecuteContextSwitch(lpNewCalc, STAT_PLOT_CONTEXT);
}

static void GotoWindowScreen(LPCALC lpNewCalc) {
	ExecuteContextSwitch(lpNewCalc, WINDOW_CONTEXT);
}

static void GotoTableSetScreen(LPCALC lpNewCalc) {
	ExecuteContextSwitch(lpNewCalc, TABLESET_CONTEXT);
}


static void GotoGraphScreen(LPCALC lpNewCalc) {
	uint8_t code[8] = { 0x3E, 0x44, 0xEF, 02, 0x50, 0xEF, 0x1E, 0x40 };
	for (int i = 0; i < ARRAYSIZE(code); i++) {
		mem_write(&lpNewCalc->mem_c, (uint16_t)(APPBACKUPSCREEN + i), code[i]);
	}

	ExecuteCustomCode(lpNewCalc);
}

static void GotoTableScreen(LPCALC lpNewCalc) {
	ExecuteContextSwitch(lpNewCalc, TABLE_CONTEXT);
}

static void GotoListScreen(LPCALC lpNewCalc) {
	ExecuteContextSwitch(lpNewCalc, LIST_CONTEXT);
}

#define BASE_ID 2020
LRESULT CALLBACK TeacherViewProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	static HFONT hfontSegoe = NULL;
	static TEXTMETRIC tm;

	LPMAINWINDOW lpMainWindow = (LPMAINWINDOW) GetWindowLongPtr(hwnd, GWLP_USERDATA);
	switch (msg) {
	case WM_CREATE: {
		lpMainWindow = (LPMAINWINDOW)((LPCREATESTRUCT)lParam)->lpCreateParams;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)lpMainWindow);

		if (hfontSegoe == NULL) {
			NONCLIENTMETRICS ncm = { 0 };
			ncm.cbSize = sizeof(ncm);
			SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0);

			hfontSegoe = CreateFontIndirect(&ncm.lfMessageFont);
		}

		HDC hdc = GetDC(hwnd);
		SelectObject(hdc, hfontSegoe);
		GetTextMetrics(hdc, &tm);
		ReleaseDC(hwnd, hdc);

		int dpi = GetDpiForWindow(hwnd);

		LPCALC lpCalc = lpMainWindow->lpCalc;
		LONG lcd_width = MulDiv(lpCalc->cpu.pio.lcd->display_width * TEACHER_VIEW_SCALE, dpi, 96);
		LONG lcd_height = MulDiv(lpCalc->cpu.pio.lcd->height * TEACHER_VIEW_SCALE, dpi, 96);
		int i = BASE_ID;

		for (int row = 0; row < TEACHER_VIEW_ROWS; row++) {
			for (int col = 0; col < TEACHER_VIEW_COLS; col++) {
				TCHAR *name = _T("View Type: ");
				LONG x = lcd_width * col + tm.tmAveCharWidth;
				LONG y = (lcd_height * (row + 1)) + MulDiv((TEACHER_VIEW_CAPTION_SIZE * row) +
					(TEACHER_VIEW_CAPTION_SIZE / 4), dpi, 96);
				LONG width = tm.tmAveCharWidth * _tcslen(name);
				LONG height = tm.tmHeight;
				HWND hwndNewStatic = CreateWindow(WC_STATIC, name, WS_CHILD | WS_VISIBLE, 
					x, y, width, height, hwnd, NULL, g_hInst, NULL);
				SetWindowFont(hwndNewStatic, hfontSegoe, TRUE);

				HWND hwndNewCombo = CreateWindow(WC_COMBOBOX, _T(""), 
					WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | CBS_HASSTRINGS,
					x + width, y - 1, tm.tmAveCharWidth * 10, height, hwnd, (HMENU)i++, g_hInst, NULL);
				SetWindowFont(hwndNewCombo, hfontSegoe, TRUE);
				ComboBox_AddString(hwndNewCombo, _T("Y="));
				ComboBox_AddString(hwndNewCombo, _T("Stat Plot"));
				ComboBox_AddString(hwndNewCombo, _T("Window"));
				ComboBox_AddString(hwndNewCombo, _T("Table Set"));
				ComboBox_AddString(hwndNewCombo, _T("Graph"));
				ComboBox_AddString(hwndNewCombo, _T("Table"));
				ComboBox_AddString(hwndNewCombo, _T("List"));
				ComboBox_SetCurSel(hwndNewCombo, (int)lpMainWindow->teacher_views[col][row]);
			}
		}

		return FALSE;
	}
	case WM_COMMAND: {
		if (HIWORD(wParam) == CBN_SELCHANGE) {
			int id = LOWORD(wParam) - BASE_ID;
			int row = id / TEACHER_VIEW_ROWS;
			int col = id % TEACHER_VIEW_COLS;
			HWND hwndCombo = (HWND)lParam;
			lpMainWindow->teacher_views[col][row] = (TeacherViewScreen_t) ComboBox_GetCurSel(hwndCombo);

			LPCALC lpCalc = lpMainWindow->lpCalc;
			int scale = lpCalc->model < TI_84PCSE ? TEACHER_VIEW_SCALE : 1;
			LCDBase_t *lcd = lpCalc->cpu.pio.lcd;
			LONG lcd_width = lcd->display_width * scale;
			LONG lcd_height = lcd->height * scale;

			RECT rc;
			rc.left = col * lcd_width;
			rc.top = (row * lcd_height) + (TEACHER_VIEW_CAPTION_SIZE * row);
			rc.right = rc.left + lcd_width;
			rc.bottom = rc.top + lcd_height;
			InvalidateRect(hwnd, &rc, FALSE);
		}
		break;
	}
	case WM_CTLCOLORSTATIC:
	case WM_CTLCOLORBTN: {
		return (LRESULT)GetSysColorBrush(COLOR_WINDOW);
	}
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		Graphics g(hdc);
		g.SetInterpolationMode(InterpolationModeNearestNeighbor);
		g.SetPixelOffsetMode(PixelOffsetModeHalf);
		COLORREF ref = GetBkColor(hdc);
		Color color(GetRValue(ref), GetGValue(ref), GetBValue(ref));
		g.Clear(color);

		if (lpMainWindow->lpTeacherViewCalc != NULL) {
			calc_slot_free(lpMainWindow->lpTeacherViewCalc);
			free(lpMainWindow->lpTeacherViewCalc);
			lpMainWindow->lpTeacherViewCalc = NULL;
		}

		lpMainWindow->lpTeacherViewCalc = DuplicateCalc(lpMainWindow->lpCalc);
		LPCALC lpNewCalc = lpMainWindow->lpTeacherViewCalc;
		if (lpNewCalc == NULL) {
			goto paintFail;
		}

		lpNewCalc->running = TRUE;
		lpNewCalc->speed = 400;

		int dpi = GetDpiForWindow(hwnd);
		int scale = lpNewCalc->model < TI_84PCSE ? TEACHER_VIEW_SCALE : 1;
		LONG lcd_width = MulDiv(lpNewCalc->cpu.pio.lcd->display_width * scale, dpi, 96);
		LONG lcd_height = MulDiv(lpNewCalc->cpu.pio.lcd->height * scale, dpi, 96);

		for (int row = 0; row < TEACHER_VIEW_ROWS; row++) {
			for (int col = 0; col < TEACHER_VIEW_COLS; col++) {
				switch (lpMainWindow->teacher_views[row][col]) {
				case YEQU_SCREEN:
					GotoYEquScreen(lpNewCalc);
					break;
				case STATPLOT_SCREEN:
					GotoStatPlotScreen(lpNewCalc);
					break;
				case WINDOW_SCREEN:
					GotoWindowScreen(lpNewCalc);
					break;
				case TABLESET_SCREEN:
					GotoTableSetScreen(lpNewCalc);
					break;
				case GRAPH_SCREEN:
					GotoGraphScreen(lpNewCalc);
					break;
				case TABLE_SCREEN:
					GotoTableScreen(lpNewCalc);
					break;
				case LIST_SCREEN:
					GotoListScreen(lpNewCalc);
					break;
				}

				calc_run_tstates(lpNewCalc, lpNewCalc->timer_c.freq / 8);
				DrawScreen(g, 
					row * lcd_width,
					(col * lcd_height) + MulDiv((TEACHER_VIEW_CAPTION_SIZE * col), dpi, 96),
					lcd_width,
					lcd_height,
					lpNewCalc);
				lpNewCalc->running = TRUE;
			}
		}

paintFail:
		EndPaint(hwnd, &ps);
		return FALSE;
	}
	case WM_USER: {
		InvalidateRect(hwnd, NULL, FALSE);
		UpdateWindow(hwnd);
		break;
	}
	case WM_DESTROY:
		lpMainWindow->hwndTeacherView = NULL;
		calc_slot_free(lpMainWindow->lpTeacherViewCalc);
		lpMainWindow->lpTeacherViewCalc = NULL;
		break;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}