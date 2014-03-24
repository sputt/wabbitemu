#include "stdafx.h"

#include "guiteacherview.h"
#include "gui.h"
#include "savestate.h"

#define SECOND_GROUP 6
#define SECOND_BIT 5

#define MODE_GROUP 6
#define MODE_BIT 6

#define YEQU_GROUP 6
#define YEQU_BIT 4

#define WINDOW_GROUP 6
#define WINDOW_BIT 3

#define GRAPH_GROUP 6
#define GRAPH_BIT 0

void DrawScreen(Graphics &graphics, int x, int y, LPCALC lpCalc) {
	LCDBase_t *lcd = lpCalc->cpu.pio.lcd;
	uint8_t *screen = lcd->image(lcd);
	LPBITMAPINFO info = GetLCDColorPalette(lpCalc->model, lcd);
	Bitmap bitmap(info, screen);

	Rect destRect(x, y, lcd->display_width * TEACHER_VIEW_SCALE, lcd->height * TEACHER_VIEW_SCALE);
	graphics.DrawImage(&bitmap, destRect, 0, 0, lcd->display_width, lcd->height, UnitPixel);
}

void GotoYEquScreen(LPCALC lpNewCalc) {
	press_key(lpNewCalc, SECOND_GROUP, SECOND_BIT);
	press_key(lpNewCalc, MODE_GROUP, MODE_BIT);
	press_key(lpNewCalc, YEQU_GROUP, YEQU_BIT);
}

void GotoStatPlotScreen(LPCALC lpNewCalc) {
	press_key(lpNewCalc, SECOND_GROUP, SECOND_BIT);
	press_key(lpNewCalc, MODE_GROUP, MODE_BIT);
	press_key(lpNewCalc, SECOND_GROUP, SECOND_BIT);
	press_key(lpNewCalc, YEQU_GROUP, YEQU_BIT);
}

void GotoWindowScreen(LPCALC lpNewCalc) {
	press_key(lpNewCalc, SECOND_GROUP, SECOND_BIT);
	press_key(lpNewCalc, MODE_GROUP, MODE_BIT);
	press_key(lpNewCalc, WINDOW_GROUP, WINDOW_BIT);
}

void GotoGraphScreen(LPCALC lpNewCalc) {
	press_key(lpNewCalc, SECOND_GROUP, SECOND_BIT);
	press_key(lpNewCalc, MODE_GROUP, MODE_BIT);
	press_key(lpNewCalc, GRAPH_GROUP, GRAPH_BIT);
}

void GotoTableScreen(LPCALC lpNewCalc) {
	press_key(lpNewCalc, SECOND_GROUP, SECOND_BIT);
	press_key(lpNewCalc, MODE_GROUP, MODE_BIT);
	press_key(lpNewCalc, SECOND_GROUP, SECOND_BIT);
	press_key(lpNewCalc, GRAPH_GROUP, GRAPH_BIT);
}

LRESULT CALLBACK TeacherViewProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	LPMAINWINDOW lpMainWindow = (LPMAINWINDOW) GetWindowLongPtr(hwnd, GWLP_USERDATA);
	switch (msg) {
	case WM_CREATE: {
		lpMainWindow = (LPMAINWINDOW)((LPCREATESTRUCT)lParam)->lpCreateParams;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)lpMainWindow);
		return FALSE;
	}
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		Graphics g(hdc);
		g.SetInterpolationMode(InterpolationModeNearestNeighbor);
		g.SetPixelOffsetMode(PixelOffsetModeHalf);

		LPCALC lpNewCalc = DuplicateCalc(lpMainWindow->lpCalc);
		lpNewCalc->running = TRUE;
		lpNewCalc->speed = 400;
		LONG lcd_width = lpNewCalc->cpu.pio.lcd->display_width * TEACHER_VIEW_SCALE;
		LONG lcd_height = lpNewCalc->cpu.pio.lcd->height * TEACHER_VIEW_SCALE;

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
				case GRAPH_SCREEN:
					GotoGraphScreen(lpNewCalc);
					break;
				case TABLE_SCREEN:
					GotoTableScreen(lpNewCalc);
					break;
				}

				u_char charFlag = mem_read(&lpNewCalc->mem_c, 0x89F0 + 0x0C);
				mem_write(&lpNewCalc->mem_c, 0x89F0 + 0x0C, charFlag & BIT(2));
				calc_run_tstates(lpNewCalc, lpNewCalc->timer_c.freq / 4);
				DrawScreen(g, row * lcd_width, col * lcd_height, lpNewCalc);
			}
		}

		EndPaint(hwnd, &ps);
		return FALSE;
	}
	case WM_DESTROY:
		lpMainWindow->hwndTeacherView = NULL;
		break;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}