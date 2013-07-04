#include "stdafx.h"

#include "dbspriteviewer.h"
#include "dbcommon.h"
#include "guidebug.h"
#include "dbwatch.h"

INT_PTR CALLBACK SpriteViewerDialogProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message) {
		case WM_INITDIALOG: {
			//LPTABWINDOWINFO
			SetWindowLongPtr(hwnd, GWLP_USERDATA, lParam);
			return FALSE;
		}
		case WM_SIZE: {
			LPDRAWITEMSTRUCT lpItem = (LPDRAWITEMSTRUCT) lParam;
			LPTABWINDOWINFO lpTabInfo = (LPTABWINDOWINFO) GetWindowLongPtr(hwnd, GWLP_USERDATA);
			HWND spriteBox = GetDlgItem(hwnd, IDC_STATIC_SPRITE);
			watchpoint_t *watchpoint = (watchpoint_t *) lpTabInfo->tabInfo;
			LPCALC lpCalc = lpTabInfo->lpDebugInfo->lpCalc;
			SetWindowPos(spriteBox, NULL, 0, 0, watchpoint->width * lpCalc->scale, watchpoint->height * lpCalc->scale, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);
			return FALSE;
		}
		case WM_DRAWITEM: {
			if (wParam == IDC_STATIC_SPRITE) {
				LPDRAWITEMSTRUCT lpItem = (LPDRAWITEMSTRUCT) lParam;
				LPTABWINDOWINFO lpTabInfo = (LPTABWINDOWINFO) GetWindowLongPtr(hwnd, GWLP_USERDATA);
				HWND spriteBox = GetDlgItem(hwnd, IDC_STATIC_SPRITE);
				watchpoint_t *watchpoint = (watchpoint_t *) lpTabInfo->tabInfo;
				waddr_t waddr = watchpoint->waddr;
				LPCALC lpCalc = lpTabInfo->lpDebugInfo->lpCalc;

				HDC hdc = CreateCompatibleDC(lpItem->hDC);
				HBITMAP hbmTemp = CreateCompatibleBitmap(lpItem->hDC, watchpoint->width * lpCalc->scale, watchpoint->height * lpCalc->scale);
				SelectObject(hdc, hbmTemp);
				DeleteObject(hbmTemp);

				for (int i = 0; i < watchpoint->height; i++) {
					for (int j = 0; j < watchpoint->width / 8; j++) {
						int mask = 0x80;
						int mem_val = wmem_read(&lpCalc->mem_c, waddr);
						for (int k = 0; k < 8; k++, mask >>= 1) {
							if (mem_val & mask) {
								SetPixel(hdc, j * 8 + k, i, RGB(0, 0, 0));
							} else {
								SetPixel(hdc, j * 8 + k, i, RGB(255, 255, 255));
							}
						}
						waddr.addr++;
					}
				}
				StretchBlt(lpItem->hDC, 0, 0, watchpoint->width * lpCalc->scale, watchpoint->height * lpCalc->scale,
					hdc, 0, 0, watchpoint->width, watchpoint->height, SRCCOPY);
			}
		}
		case WM_USER: {
			switch(wParam) {
				case DB_UPDATE:
					InvalidateRect(hwnd, NULL, TRUE);
					UpdateWindow(hwnd);
					break;

			}
			return TRUE;
		}
		case WM_CLOSE: {
			EndDialog(hwnd, S_OK);
			return TRUE;
		}
	}
	return FALSE;
}