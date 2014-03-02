#include "stdafx.h"

#include "dbspriteviewer.h"
#include "dbcommon.h"
#include "guidebug.h"
#include "dbwatch.h"
#include "colorlcd.h"

void DrawGrayImage(HDC hdc, LPCALC lpCalc, watchpoint_t *watchpoint) {
	waddr_t waddr = watchpoint->waddr;
	for (int i = 0; i < watchpoint->height; i++) {
		for (int j = 0; j < watchpoint->width / 8; j++) {
			int mask = 0x80;
			int mem_val = wmem_read(&lpCalc->mem_c, waddr);
			for (int k = 0; k < 8; k++, mask >>= 1) {
				if (mem_val & mask) {
					SetPixel(hdc, j * 8 + k, i, RGB(0, 0, 0));
				}
				else {
					SetPixel(hdc, j * 8 + k, i, RGB(255, 255, 255));
				}
			}
			waddr.addr++;
		}
	}
}

extern BITMAPINFO *colorbi;
#define TRUCOLOR(color, bits) (color) * (0xFF / ((1 << (bits)) - 1))
void DrawColorImage(HDC hdc, LPCALC lpCalc) {
	ColorLCD_t *lcd = (ColorLCD_t *)lpCalc->cpu.pio.lcd;
	uint8_t buf[COLOR_LCD_DISPLAY_SIZE];
	for (int i = 0; i < COLOR_LCD_DISPLAY_SIZE; i++) {
		buf[i] = TRUCOLOR(lcd->display[i], 6);
	}
	StretchDIBits(hdc, 0, 0, COLOR_LCD_WIDTH, COLOR_LCD_HEIGHT,
		0, 0, COLOR_LCD_WIDTH, COLOR_LCD_HEIGHT,
		buf,
		colorbi,
		DIB_RGB_COLORS,
		SRCCOPY);
}

INT_PTR CALLBACK SpriteViewerDialogProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	static BOOL hasErasedBg;
	switch (Message) {
		case WM_INITDIALOG: {
			//LPTABWINDOWINFO
			SetWindowLongPtr(hwnd, GWLP_USERDATA, lParam);
			hasErasedBg = FALSE;
			return FALSE;
		}
		case WM_SIZE: {
			LPTABWINDOWINFO lpTabInfo = (LPTABWINDOWINFO) GetWindowLongPtr(hwnd, GWLP_USERDATA);
			HWND spriteBox = GetDlgItem(hwnd, IDC_STATIC_SPRITE);
			watchpoint_t *watchpoint = (watchpoint_t *) lpTabInfo->tabInfo;
			LPCALC lpCalc = lpTabInfo->lpDebugInfo->lpCalc;
			int width, height, scale;
			if (lpCalc->model >= TI_84PCSE) {
				width = COLOR_LCD_WIDTH;
				height = COLOR_LCD_HEIGHT;
				scale = 1;
			}
			else {
				width = watchpoint->width;
				height = watchpoint->height;
				scale = 2;
			}
			SetWindowPos(spriteBox, NULL, 0, 0, width * scale, height * scale, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);
			return FALSE;
		}
		case WM_DRAWITEM: {
			if (wParam == IDC_STATIC_SPRITE) {
				LPDRAWITEMSTRUCT lpItem = (LPDRAWITEMSTRUCT) lParam;
				LPTABWINDOWINFO lpTabInfo = (LPTABWINDOWINFO) GetWindowLongPtr(hwnd, GWLP_USERDATA);
				watchpoint_t *watchpoint = (watchpoint_t *) lpTabInfo->tabInfo;
				LPCALC lpCalc = lpTabInfo->lpDebugInfo->lpCalc;

				int width, height, scale;
				if (lpCalc->model >= TI_84PCSE) {
					width = COLOR_LCD_WIDTH;
					height = COLOR_LCD_HEIGHT;
					scale = 1;
				} else {
					width = watchpoint->width;
					height = watchpoint->height;
					scale = 2;
				}

				HDC hdc = CreateCompatibleDC(lpItem->hDC);
				HBITMAP hbmTemp = CreateCompatibleBitmap(lpItem->hDC, width * scale, height * scale);
				SelectObject(hdc, hbmTemp);
				DeleteObject(hbmTemp);

				if (lpCalc->model >= TI_84PCSE) {
					DrawColorImage(lpItem->hDC, lpCalc);
				} else {
					DrawGrayImage(hdc, lpCalc, watchpoint);
					StretchBlt(lpItem->hDC, 0, 0, width * scale, height * scale,
						hdc, 0, 0, width, height, SRCCOPY);
				}
			}
		}
		case WM_USER: {
			switch(wParam) {
				case DB_UPDATE:
					InvalidateRect(hwnd, NULL, FALSE);
					UpdateWindow(hwnd);
					break;

			}
			return TRUE;
		}
		case WM_CLOSE: {
			SendMessage(GetParent(hwnd), WM_USER, DB_SPRITE_CLOSE, 0);
			EndDialog(hwnd, S_OK);
			return TRUE;
		}
		case WM_DESTROY: {
			LPTABWINDOWINFO lpTabInfo = (LPTABWINDOWINFO)GetWindowLongPtr(hwnd, GWLP_USERDATA);
			free(lpTabInfo);
			return TRUE;
		}
	}
	return FALSE;
}