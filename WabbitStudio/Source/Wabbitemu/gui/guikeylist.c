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
			hImageList = ImageList_Create(48, 48, ILC_COLOR32 | ILC_MASK, 8*7, 0);
			lpCalc = (LPCALC) GetWindowLongPtr(GetParent(hwnd), GWLP_USERDATA);

			HDC hdcMask = CreateCompatibleDC(lpCalc->hdcButtons);
			HBITMAP hbmMask = CreateCompatibleBitmap(lpCalc->hdcButtons, 48, 48);
			SelectObject(hdcMask, hbmMask);

			HDC hdcButton = CreateCompatibleDC(lpCalc->hdcButtons);
			HBITMAP hbmButton  = CreateCompatibleBitmap(lpCalc->hdcButtons, 48, 48);
			SelectObject(hdcButton, hbmButton);

			for(int group = 0; group < 7; group++) {
				for(int bit = 0; bit < 8; bit++) {
					if ((*ButtonCenter)[bit + (group << 3)].x != 0xFFF) {
						pt.x	= (*ButtonCenter)[bit + (group << 3)].x;
						pt.y	= (*ButtonCenter)[bit + (group << 3)].y;


						HDC hdc = CreateCompatibleDC(lpCalc->hdcButtons);
						HBITMAP hbm = CreateCompatibleBitmap(lpCalc->hdcButtons, 48, 48);
						HGDIOBJ hbmOld = SelectObject(hdc, hbm);

						RECT r = {0, 0, 48, 48};
						SetDCBrushColor(hdc, RGB(0, 255, 0));
						FillRect(hdc, &r, GetStockBrush(DC_BRUSH));
						
						//BitBlt(hdc, 0, 0, 48, 48, lpCalc->hdcButtons, pt.x - 24, pt.y - 24, SRCCOPY);
						DrawButtonStateNoSkin(hdc, lpCalc->hdcSkin, lpCalc->hdcKeymap, &pt, DBS_COPY);

						SelectObject(hdc, hbmOld);
						DeleteDC(hdc);

						//hbmButton = DrawButtonAndMask(lpCalc, pt, &hbmButton, &hbmMask);
						ImageList_AddMasked(hImageList, hbm, RGB(0, 255, 0));
						DeleteObject(hbmButton);
					}
				}
			}
			ListView_SetView(hListKeys, LV_VIEW_ICON);
			ListView_SetImageList(hListKeys, hImageList, LVSIL_NORMAL);
			LVITEM lItem;
			lItem.mask = LVIF_IMAGE | LVIF_TEXT;
			lItem.iSubItem = 0;
			lItem.iImage = 1;
			lItem.iItem = 0;
			lItem.pszText = _T("Test");
			ListView_InsertItem(hListKeys, &lItem);

			lItem.iImage = 2;
			lItem.iItem = 1;
			lItem.pszText = _T("Test2");
			ListView_InsertItem(hListKeys, &lItem);
			return TRUE;
		}
		case WM_COMMAND:
			switch (HIWORD(wParam)) {
				case BN_CLICKED:
					ListView_DeleteAllItems(hListKeys);
					break;
			}
		case WM_USER:
			//TODO: allow exporting by writing group/bit data and outputting to a text file
			if (wParam == ADD_LISTBOX_ITEM) {
				if (ListBox_GetCount(hListKeys) > MAX_KEYPRESS_HISTORY)
					ListBox_DeleteString(hListKeys, 0);
				ListBox_AddString(hListKeys, (TCHAR *) lParam);
			}
			return TRUE;
		case WM_CLOSE:
			DestroyWindow(hwnd);
			return TRUE;
	}
	return FALSE;
}