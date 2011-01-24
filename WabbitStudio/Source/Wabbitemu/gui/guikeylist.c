#include "guikeylist.h"
#include "gui.h"
#include "resource.h"

LRESULT CALLBACK KeysListProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	static HWND hListKeys;
	switch (uMsg) {
		case WM_INITDIALOG:
			hListKeys = GetDlgItem(hwnd, IDC_LISTBOX_KEYS);
			return TRUE;
		case WM_COMMAND:
			switch (HIWORD(wParam)) {
				case BN_CLICKED:
					ListBox_ResetContent(hListKeys);
					break;
			}
		case WM_USER:
			//TODO: allow exporting by writing group/bit data and outputing to a text file
			if (wParam == ADD_LISTBOX_ITEM) {
				if (ListBox_GetCount(hListKeys) > MAX_KEYPRESS_HISTORY)
					ListBox_DeleteString(hListKeys, 0);
				ListBox_AddString(hListKeys, (TCHAR *) lParam);
			}
			return TRUE;
		case WM_CLOSE:

			return TRUE;
	}
	return FALSE;
}