#include "stdafx.h"

#include "fileutilities.h"
#include "guikeylist.h"
#include "guibuttons.h"
#include "gui.h"
#include "resource.h"

LRESULT CALLBACK KeysListProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	static HWND hListKeys;
	static HIMAGELIST hImageList;
	static LPCALC lpCalc;
	extern POINT *ButtonCenter[64];
	switch (uMsg) {
		case WM_INITDIALOG: {
			POINT pt;
			hListKeys = GetDlgItem(hwnd, IDC_LISTVIEW_KEYS);
			hImageList = ImageList_Create(48, 32, ILC_COLOR32 | ILC_MASK, 8*7, 0);
			lpCalc = (LPCALC) GetWindowLongPtr(GetParent(hwnd), GWLP_USERDATA);

			HDC hdcMask = CreateCompatibleDC(lpCalc->hdcButtons);
			HBITMAP hbmMask = CreateCompatibleBitmap(lpCalc->hdcButtons, 48, 32);
			SelectObject(hdcMask, hbmMask);

			HDC hdcButton = CreateCompatibleDC(lpCalc->hdcButtons);
			HBITMAP hbmButton  = CreateCompatibleBitmap(lpCalc->hdcButtons, 48, 32);
			SelectObject(hdcButton, hbmButton);

			for(int group = 0; group < 7; group++) {
				for(int bit = 0; bit < 8; bit++) {
					if ((*ButtonCenter)[bit + (group << 3)].x != 0xFFF) {
						pt.x	= (*ButtonCenter)[bit + (group << 3)].x;
						pt.y	= (*ButtonCenter)[bit + (group << 3)].y;


						HDC hdc = CreateCompatibleDC(lpCalc->hdcButtons);
						HBITMAP hbm = CreateCompatibleBitmap(lpCalc->hdcButtons, 48, 32);
						HGDIOBJ hbmOld = SelectObject(hdc, hbm);

						RECT r = {0, 0, 48, 32};
						SetDCBrushColor(hdc, RGB(0, 255, 0));
						FillRect(hdc, &r, GetStockBrush(DC_BRUSH));
						
						DrawButtonStateNoSkin(hdc, lpCalc->hdcSkin, lpCalc->hdcKeymap, &pt, DBS_COPY);
						DrawButtonShadow(hdc, lpCalc->hdcKeymap, &pt);

						SelectObject(hdc, hbmOld);
						DeleteDC(hdc);

						ImageList_AddMasked(hImageList, hbm, RGB(0, 255, 0));
						DeleteObject(hbmButton);
					}
				}
			}
			ListView_SetView(hListKeys, LV_VIEW_TILE);
			SIZE size = { 56, 64 };
			LVTILEVIEWINFO ltvi = {0};
			ltvi.cbSize   = sizeof(ltvi);
			ltvi.dwFlags  = LVTVIF_FIXEDSIZE;
			ltvi.dwMask   = LVTVIM_TILESIZE;
			ltvi.sizeTile = size;

			ListView_SetTileViewInfo(hListKeys, &ltvi);
			ListView_SetImageList(hListKeys, hImageList, LVSIL_NORMAL);
			SetWindowTheme(hListKeys, L"explorer", NULL);

			SendMessage(hwnd, WM_USER, REFRESH_LISTVIEW, 0);
			return TRUE;
		}
		case WM_COMMAND: {
			switch (HIWORD(wParam)) {
				case BN_CLICKED: {
					switch (LOWORD(wParam)) {
						case IDC_BTN_CLEARKEYS: {
							key_string *current = lpCalc->last_keypress_head;
							while (current) {
								free(current->text);
								key_string *next = current->next;
								free(current);
								current = next;
							}
							lpCalc->last_keypress_head = NULL;
							ListView_DeleteAllItems(hListKeys);
							break;
						}
						case IDC_BTN_SAVEKEYS:
							TCHAR lpStrFile[MAX_PATH];
							const TCHAR lpstrFilter[] = _T("Recorded key files ( *.key) \0*.key\0\
														All Files (*.*)\0*.*\0\0");
							if (SaveFile(lpStrFile, lpstrFilter, _T("Save key file"), _T(".key"))) {
								FILE *file;
								fopen_s(&file, lpStrFile, _T("wb"));
								key_string *current = lpCalc->last_keypress_head;
								while (current) {
									fprintf_s(file, _T("Bit: %d Group %d\n"), current->bit, current->group);
									current = current->next;
								}
							}
							break;
						}
					}
					break;
			}
		}
		case WM_NOTIFY: {
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
						key_string *current = lpCalc->last_keypress_head;
						key_string *prev = current;
						for (int i = 0; i < lpCalc->num_keypresses - index - 1; i++) {
							prev = current;
							//assuming our linked list is good
							current = current->next;
						}
						if (current == lpCalc->last_keypress_head) {
							free(current->text);
							lpCalc->last_keypress_head = current->next;
							free(current);
						} else {
							free(current->text);
							prev->next = current->next;
							free(current);				
						}
						lpCalc->num_keypresses--;
						index = ListView_GetNextItem(hListKeys, -1, LVNI_SELECTED);
					}
					SendMessage(hwnd, WM_USER, REFRESH_LISTVIEW, 0);
					break;
				}
			}
			break;
		}
		case WM_USER:
			//TODO: allow exporting by writing group/bit data and outputting to a text file
			if (wParam == REFRESH_LISTVIEW) {
				ListView_DeleteAllItems(hListKeys);
				key_string *current = lpCalc->last_keypress_head;
				while (current) {
					LVITEM lItem;
					lItem.mask = LVIF_IMAGE;
					lItem.iSubItem = 0;
					lItem.iImage = current->group * 8 + current->bit;
					lItem.iItem = 0;
					ListView_InsertItem(hListKeys, &lItem);

					current = current->next;
				}
			}
			return TRUE;
		case WM_CLOSE:
			DestroyWindow(hwnd);
			return TRUE;
	}
	return FALSE;
}