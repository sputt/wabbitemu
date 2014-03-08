#include "stdafx.h"

#include "fileutilities.h"
#include "guikeylist.h"
#include "guibuttons.h"
#include "gui.h"

static HIMAGELIST hImageList = NULL;
extern HINSTANCE g_hInst;

LRESULT CALLBACK KeysListProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	LPMAINWINDOW lpMainWindow = (LPMAINWINDOW)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	LPCALC lpCalc = NULL;
	if (lpMainWindow != NULL) {
		lpCalc = lpMainWindow->lpCalc;
	}


	switch (uMsg) {
		case WM_INITDIALOG: {
			HICON hIcon = LoadIcon(g_hInst, _T("w"));
			SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)	hIcon);
			DeleteObject(hIcon);

			HWND hListKeys = GetDlgItem(hwnd, IDC_LISTVIEW_KEYS);
			lpMainWindow = (LPMAINWINDOW)lParam;
			lpCalc = lpMainWindow->lpCalc;
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)lpMainWindow);

			if (hImageList) {
				ImageList_Destroy(hImageList);
			}

#define KEYLIST_BUTTON_WIDTH 60
#define KEYLIST_BUTTON_HEIGHT 60
			POINT pt;
			hImageList = ImageList_Create(KEYLIST_BUTTON_WIDTH, KEYLIST_BUTTON_HEIGHT, ILC_COLOR32 | ILC_MASK, 8 * 7, 0);

			RECT r = { 0, 0, KEYLIST_BUTTON_WIDTH, KEYLIST_BUTTON_HEIGHT };
			RECT brect;

			LONG largestWidth = 0;
			LONG largestHeight = 0;

			for (int group = 0; group < 7; group++) {
				for (int bit = 0; bit < 8; bit++) {
					extern RECT ButtonRect[64];
					brect = ButtonRect[bit + (group << 3)];
					UINT keymap_scale = (UINT)(1.0 / lpMainWindow->default_skin_scale);

					LONG rectWidth = brect.right - brect.left;
					LONG rectHeight = brect.bottom - brect.top;
					LONG scaleWidth = rectWidth * 3 / 2 / keymap_scale;
					LONG scaleHeight = rectHeight * 3 / 2 / keymap_scale;

					if (scaleWidth > largestWidth) {
						largestWidth = scaleWidth;
					}
					if (scaleHeight > largestHeight) {
						largestHeight = scaleHeight;
					}
					
					Bitmap bitmapButton(rectWidth, rectHeight, PixelFormat32bppARGB);
					DrawButtonStateNoSkin(&bitmapButton, lpMainWindow->m_lpBitmapSkin, lpMainWindow->m_lpBitmapKeymap, brect, DBS_COPY);

					Bitmap scaledButton(KEYLIST_BUTTON_WIDTH, KEYLIST_BUTTON_HEIGHT);
					Graphics g(&scaledButton);
					g.SetInterpolationMode(InterpolationModeHighQuality);
					Rect destRect((r.right - scaleWidth) / 2, (r.bottom - scaleHeight) / 2, scaleWidth, scaleHeight);
					g.DrawImage(&bitmapButton, destRect, 0, 0, rectWidth, rectHeight, UnitPixel);
					HBITMAP hbm;
					scaledButton.GetHBITMAP(Color::Lime, &hbm);

					ImageList_AddMasked(hImageList, hbm, RGB(0, 255, 0));

					DeleteObject(hbm);
				}
			}

			ListView_SetView(hListKeys, LV_VIEW_TILE);
			SIZE size = { largestWidth + 10, largestHeight + 10 };
			LVTILEVIEWINFO ltvi = {0};
			ltvi.cbSize   = sizeof(ltvi);
			ltvi.dwFlags  = LVTVIF_FIXEDSIZE;
			ltvi.dwMask   = LVTVIM_TILESIZE;
			ltvi.sizeTile = size;
			ListView_SetTileViewInfo(hListKeys, &ltvi);

			ListView_SetColumnWidth(hwnd, 0, LVSCW_AUTOSIZE);

			if (hImageList != ListView_GetImageList(hListKeys, LVSIL_NORMAL)) {
				ListView_SetImageList(hListKeys, hImageList, LVSIL_NORMAL);
			}
			SetWindowTheme(hListKeys, L"explorer", NULL);

			SendMessage(hwnd, WM_USER, REFRESH_LISTVIEW, 0);
			SendMessage(hwnd, WM_SIZE, 0, 0);
			return TRUE;
		}
		case WM_SIZE: {
			RECT rc;
			GetClientRect(hwnd, &rc);
			HWND hListKeys = GetDlgItem(hwnd, IDC_LISTVIEW_KEYS);
			SetWindowPos(hListKeys, NULL, 0, 0, rc.right - 10 - 10, rc.bottom - 10 - 40, SWP_NOMOVE | SWP_NOZORDER);
			SetWindowPos(GetDlgItem(hwnd, IDC_BTN_CLEARKEYS), NULL, 10, rc.bottom - 40 + 5, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
			SetWindowPos(GetDlgItem(hwnd, IDC_BTN_SAVEKEYS), NULL, 130, rc.bottom - 40 + 5, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
			SendMessage(hwnd, WM_USER, REFRESH_LISTVIEW, 0);
			break;
		}
		case WM_COMMAND: {
			HWND hListKeys = GetDlgItem(hwnd, IDC_LISTVIEW_KEYS);
			switch (HIWORD(wParam)) {
				case BN_CLICKED: {
					switch (LOWORD(wParam)) {
						case IDC_BTN_CLEARKEYS: {
							lpMainWindow->keys_pressed->clear();
							
							ListView_DeleteAllItems(hListKeys);
							break;
						}
						case IDC_BTN_SAVEKEYS:
							TCHAR lpStrFile[MAX_PATH];
							const TCHAR lpstrFilter[] = _T("Recorded key files ( *.key) \0*.key\0\
														All Files (*.*)\0*.*\0\0");
							if (!SaveFile(lpStrFile, lpstrFilter, _T("Save key file"), _T(".key"), 0, 0)) {
								FILE *file;
								_tfopen_s(&file, lpStrFile, _T("wb"));
								for (auto it = lpMainWindow->keys_pressed->begin(); it != lpMainWindow->keys_pressed->end(); it++) {
									key_string_t current = *it;
									_ftprintf_s(file, _T("Bit: %d Group: %d\r\n"), current.bit, current.group);
								}
								fclose(file);
							}
							break;
						}
					}
					break;
			}
			return 0;
		}
		case WM_NOTIFY: {
			HWND hListKeys = GetDlgItem(hwnd, IDC_LISTVIEW_KEYS);
			switch (((LPNMHDR)lParam)->code)
			{
				case LVN_KEYDOWN: {
					LPNMLVKEYDOWN pnkd = (LPNMLVKEYDOWN) lParam; 
					if (pnkd->wVKey != VK_DELETE) {
						break;
					}
					int index = ListView_GetNextItem(hListKeys, -1, LVNI_SELECTED);
					while (index != -1) {
						ListView_DeleteItem(hListKeys, index);
						int i = 0;
						for (auto it = lpMainWindow->keys_pressed->begin(); it != lpMainWindow->keys_pressed->end(); it++) {
							if (i++ == index) {
								lpMainWindow->keys_pressed->erase(it);
								break;
							}
						}

						index = ListView_GetNextItem(hListKeys, -1, LVNI_SELECTED);
					}

					SendMessage(hwnd, WM_USER, REFRESH_LISTVIEW, 0);
					break;
				}
			}
			return 0;
		}
		case WM_USER: {
			HWND hListKeys = GetDlgItem(hwnd, IDC_LISTVIEW_KEYS);
			if (wParam == REFRESH_LISTVIEW) {
				ListView_DeleteAllItems(hListKeys);
				int i = 0;
				for (auto it = lpMainWindow->keys_pressed->begin(); it != lpMainWindow->keys_pressed->end(); it++) {
					key_string_t current = *it;
					LVITEM lItem;
					lItem.mask = LVIF_IMAGE;
					lItem.iSubItem = 0;
					lItem.iImage = (current.group << 3)+ current.bit;
					lItem.iItem = i++;
					int error = ListView_InsertItem(hListKeys, &lItem);
					if (error == -1) {
						error = GetLastError();
					}
				}
			}

			return TRUE;
		}
		case WM_CLOSE:
			DestroyWindow(hwnd);
			return TRUE;
	}
	return FALSE;
}