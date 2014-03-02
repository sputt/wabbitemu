#include "stdafx.h"

#include "fileutilities.h"
#include "guikeylist.h"
#include "guibuttons.h"
#include "gui.h"

HIMAGELIST hImageList = NULL;
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
			POINT pt;
			hImageList = ImageList_Create(60, 60, ILC_COLOR32 | ILC_MASK, 8*7, 0);

			HDC hdcMask = CreateCompatibleDC(lpMainWindow->hdcButtons);
			HBITMAP hbmMask = CreateCompatibleBitmap(lpMainWindow->hdcButtons, 60, 60);
			SelectObject(hdcMask, hbmMask);

			HDC hdcButton = CreateCompatibleDC(lpMainWindow->hdcButtons);
			HBITMAP hbmButton = CreateCompatibleBitmap(lpMainWindow->hdcButtons, 60, 60);
			SelectObject(hdcButton, hbmButton);

			RECT r = {0, 0, 60, 60};
			HBRUSH br = CreateSolidBrush(RGB(0, 255, 0));
			RECT brect;

			for (int group = 0; group < 7; group++) {
				for (int bit = 0; bit < 8; bit++) {
					extern RECT ButtonRect[64];
					brect = ButtonRect[bit + (group << 3)];
					if (brect.left != 0) {
						HDC hdcButtons = CreateCompatibleDC(lpMainWindow->hdcButtons);
						HBITMAP hbm = CreateCompatibleBitmap(lpMainWindow->hdcButtons, 64, 64);
						HGDIOBJ hbmOld = SelectObject(hdcButtons, hbm);

						FillRect(hdcButtons, &r, br);

						UINT keymap_scale = (UINT)(1.0 / lpMainWindow->default_skin_scale);
						DrawButtonStateNoSkin(hdcButtons, lpMainWindow->hdcSkin, lpMainWindow->hdcKeymap, brect, DBS_COPY, keymap_scale);
						DrawButtonShadow(hdcButtons, lpMainWindow->hdcKeymap, brect, keymap_scale);

						SelectObject(hdcButtons, hbmOld);
						DeleteDC(hdcButtons);

						ImageList_AddMasked(hImageList, hbm, RGB(0, 255, 0));
						DeleteObject(hbmButton);
					}
				}
			}

			ListView_SetView(hListKeys, LV_VIEW_TILE);
			SIZE size = { 70, 70 };
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
							key_string *current = lpMainWindow->last_keypress_head;
							while (current) {
								free(current->text);
								key_string *next = current->next;
								free(current);
								current = next;
							}

							lpMainWindow->last_keypress_head = NULL;
							lpMainWindow->num_keypresses = 0;
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
								key_string *current = lpMainWindow->last_keypress_head;
								while (current) {
									_ftprintf_s(file, _T("Bit: %d Group: %d\r\n"), current->bit, current->group);
									current = current->next;
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
						key_string *current = lpMainWindow->last_keypress_head;
						key_string *prev = current;
						for (int i = 0; i < lpMainWindow->num_keypresses - index - 1; i++) {
							prev = current;
							current = current->next;
						}
						if (current == lpMainWindow->last_keypress_head) {
							free(current->text);
							lpMainWindow->last_keypress_head = current->next;
							free(current);
						} else {
							free(current->text);
							prev->next = current->next;
							free(current);				
						}

						lpMainWindow->num_keypresses--;
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
			//TODO: allow exporting by writing group/bit data and outputting to a text file
			/*if (wParam == REFRESH_LISTVIEW) {
				ListView_DeleteAllItems(hListKeys);
				key_string *current = lpCalc->last_keypress_head;
				while (current) {
					LVITEM lItem;
					lItem.mask = LVIF_IMAGE;
					lItem.iSubItem = 0;
					lItem.iImage = current->group * 8 + current->bit;
					lItem.iItem = 0;
					int error = ListView_InsertItem(hListKeys, &lItem);
					if (error == -1) {
						error = GetLastError();
					}

					current = current->next;
				}
			}*/
			if (lpMainWindow->num_keypresses >= MAX_KEYPRESS_HISTORY) {
				ListView_DeleteItem(hListKeys, 0);
			}
			key_string *current = lpMainWindow->last_keypress_head;
			if (current) {
				LVITEM lItem;
				lItem.mask = LVIF_IMAGE;
				lItem.iSubItem = 0;
				lItem.iImage = current->group * 8 + current->bit;
				lItem.iItem = MAX_KEYPRESS_HISTORY;
				int error = ListView_InsertItem(hListKeys, &lItem);
			}
			return TRUE;
		}
		case WM_CLOSE:
			DestroyWindow(hwnd);
			return TRUE;
	}
	return FALSE;
}