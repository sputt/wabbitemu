#include "stdafx.h"

#include "fileutilities.h"
#include "guikeylist.h"
#include "guibuttons.h"
#include "gui.h"
#include "sendfileswindows.h"

static HIMAGELIST hImageList = NULL;
static BOOL replayRunning = FALSE;
extern HINSTANCE g_hInst;


DWORD CALLBACK ReplayKeypressThread(LPVOID lpParam) {
	if (replayRunning) {
		return 0;
	}

	replayRunning = TRUE;

	LPMAINWINDOW lpMainWindow = (LPMAINWINDOW)lpParam;
	LPCALC lpCalc = lpMainWindow->lpCalc;

	WaitForCalcFileSendThread(lpCalc);

	uint64_t prevTimestamp = 0;
	for (auto it = lpMainWindow->keys_pressed->begin(); it != lpMainWindow->keys_pressed->end(); it++) {
		Sleep(lpMainWindow->key_playback_delay);
		press_key(lpCalc, it->group, it->bit);
	}

	replayRunning = FALSE;

	return 0;
}

int LoadKeyFile(TCHAR *filename, LPMAINWINDOW lpMainWindow) {
	FILE *file;
	_tfopen_s(&file, filename, _T("rb"));
	if (file == NULL) {
		return -1;
	}

	lpMainWindow->keys_pressed->clear();
	while (true) {
		key_string_t key;
		if (_ftscanf_s(file, _T("Bit: %d Group: %d\r\n"), &key.bit, &key.group) != 2) {
			break;
		}

		lpMainWindow->keys_pressed->push_back(key);
	}
	fclose(file);

	return 0;
}

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

			HWND replayDelay = GetDlgItem(hwnd, IDC_SLIDER_REPLAYDELAY);
			SendMessage(replayDelay, TBM_SETRANGEMIN, true, 50);
			SendMessage(replayDelay, TBM_SETRANGEMAX, true, 1000);

			if (hImageList) {
				ImageList_Destroy(hImageList);
			}

#define KEYLIST_BUTTON_WIDTH 60
#define KEYLIST_BUTTON_HEIGHT 60
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
					scaledButton.GetHBITMAP((ARGB)Color::Lime, &hbm);

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
#define BOTTOM_BAR_HEIGHT 40
#define BOTTOM_BAR_MARGIN 5
			SetWindowPos(hListKeys, NULL, 0, 0, rc.right - 10 - 10, rc.bottom - 10 - BOTTOM_BAR_HEIGHT * 2, SWP_NOMOVE | SWP_NOZORDER);
			SetWindowPos(GetDlgItem(hwnd, IDC_BTN_CLEARKEYS), NULL, 10, rc.bottom - BOTTOM_BAR_HEIGHT * 2 + BOTTOM_BAR_MARGIN, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
			SetWindowPos(GetDlgItem(hwnd, IDC_BTN_SAVEKEYS), NULL, 130, rc.bottom - BOTTOM_BAR_HEIGHT * 2 + BOTTOM_BAR_MARGIN, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
			SetWindowPos(GetDlgItem(hwnd, IDC_BTN_LOADKEYS), NULL, 250, rc.bottom - BOTTOM_BAR_HEIGHT * 2 + BOTTOM_BAR_MARGIN, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

			SetWindowPos(GetDlgItem(hwnd, IDC_BTN_REPLAYKEYS), NULL, 10, rc.bottom - BOTTOM_BAR_HEIGHT  + BOTTOM_BAR_MARGIN, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
			SetWindowPos(GetDlgItem(hwnd, IDC_REPLAYDELAY_LABEL), NULL, 130, rc.bottom - BOTTOM_BAR_HEIGHT + BOTTOM_BAR_MARGIN, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
			SetWindowPos(GetDlgItem(hwnd, IDC_SLIDER_REPLAYDELAY), NULL, 190, rc.bottom - BOTTOM_BAR_HEIGHT + BOTTOM_BAR_MARGIN, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
			SetWindowPos(GetDlgItem(hwnd, IDC_CHECK_RECORDKEYS), NULL, 310, rc.bottom - BOTTOM_BAR_HEIGHT + BOTTOM_BAR_MARGIN, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
			SendMessage(hwnd, WM_USER, REFRESH_LISTVIEW, 0);
			break;
		}
		case WM_HSCROLL: {
			HWND replayDelay = GetDlgItem(hwnd, IDC_SLIDER_REPLAYDELAY);
			if ((HWND)lParam == replayDelay) {
				lpMainWindow->key_playback_delay = SendMessage(replayDelay, TBM_GETPOS, 0, 0);
			}
			break;
		}
		case WM_COMMAND: {
			HWND hListKeys = GetDlgItem(hwnd, IDC_LISTVIEW_KEYS);
			switch (HIWORD(wParam)) {
				case BN_CLICKED: {
					switch (LOWORD(wParam)) {
						case IDC_CHECK_RECORDKEYS:
							lpMainWindow->keylogging_enabled = !lpMainWindow->keylogging_enabled;
							SendMessage(hwnd, WM_USER, REFRESH_LISTVIEW, 0);
							break;
						case IDC_BTN_CLEARKEYS: {
							lpMainWindow->keys_pressed->clear();
							
							ListView_DeleteAllItems(hListKeys);
							break;
						}
						case IDC_BTN_REPLAYKEYS: {
							CreateThread(NULL, 0, ReplayKeypressThread, lpMainWindow, 0, NULL);
							break;
						}
						case IDC_BTN_SAVEKEYS:
						case IDC_BTN_LOADKEYS:
							const TCHAR lpstrFilter[] = _T("Recorded key files ( *.key) \0*.key\0\
														All Files (*.*)\0*.*\0\0");

							WORD selection = LOWORD(wParam);
							if (selection == IDC_BTN_SAVEKEYS) {
								TCHAR lpStrFile[MAX_PATH];
								if (!SaveFile(lpStrFile, lpstrFilter, _T("Save key file"), _T(".key"), 0, 0)) {
									FILE *file;
									_tfopen_s(&file, lpStrFile, _T("wb"));
									uint64_t firstTimestamp = 0;
									uint64_t prevTimestamp = 0;
									for (auto it = lpMainWindow->keys_pressed->begin(); it != lpMainWindow->keys_pressed->end(); it++) {
										_ftprintf_s(file, _T("Bit: %d Group: %d\r\n"), it->bit, it->group);
									}
									fclose(file);
								}
							}
							else if(selection == IDC_BTN_LOADKEYS) {
								TCHAR lpStrFile[MAX_PATH];
								if (!BrowseFile(lpStrFile, lpstrFilter, _T("Load key file"), _T(".key"), 0, 0)) {
									if (!LoadKeyFile(lpStrFile, lpMainWindow)) {
										ListView_DeleteAllItems(hListKeys);
										SendMessage(hwnd, WM_USER, REFRESH_LISTVIEW, 0);
									}
								}
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
					LVITEM lItem;
					lItem.mask = LVIF_IMAGE;
					lItem.iSubItem = 0;
					lItem.iImage = (it->group << 3)+ it->bit;
					lItem.iItem = i++;
					int error = ListView_InsertItem(hListKeys, &lItem);
					if (error == -1) {
						error = GetLastError();
					}
				}

				Button_SetCheck(GetDlgItem(hwnd, IDC_CHECK_RECORDKEYS), lpMainWindow->keylogging_enabled);
				SendMessage(GetDlgItem(hwnd, IDC_SLIDER_REPLAYDELAY), TBM_SETPOS, true, lpMainWindow->key_playback_delay);
			}

			return TRUE;
		}
		case WM_CLOSE:
			DestroyWindow(hwnd);
			return TRUE;
	}
	return FALSE;
}