#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <strsafe.h>
#include "resource.h"
#include "TileSets.h"
#include "MapView.h"

extern HINSTANCE g_hInstance;

static BOOL TilesetsCallback(TCHAR *szName, LPVOID lpParam) {
	ListBox_AddString((HWND) lpParam, szName);
	return TRUE;
}

static INT_PTR CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg)
	{
	case WM_INITDIALOG:
		{
			LPMAPVIEWSETTINGS lpmvs = (LPMAPVIEWSETTINGS) lParam;
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR) lpmvs);

			EnumTilesets(TilesetsCallback, GetDlgItem(hwnd, IDC_LISTTILESETS));
			return TRUE;
		}

	case WM_COMMAND:
		switch (HIWORD(wParam))
		{
		case BN_CLICKED:
			switch (LOWORD(wParam))
			{
			case IDOK:
				{
					TCHAR szBuffer[256];
					int iSel = ListBox_GetCurSel(GetDlgItem(hwnd, IDC_LISTTILESETS));

					ListBox_GetText(GetDlgItem(hwnd, IDC_LISTTILESETS), iSel, szBuffer);
					HDC hdc = GetTileset(szBuffer);

					LPMAPVIEWSETTINGS lpmvs = (LPMAPVIEWSETTINGS) GetWindowLongPtr(hwnd, GWLP_USERDATA);
					lpmvs->hdcTiles = hdc;
					StringCbCopy(lpmvs->szTilesFilename, sizeof(lpmvs->szTilesFilename), szBuffer);
				}

			case IDCANCEL:
				EndDialog(hwnd, 0);
				return TRUE;
			}
			return FALSE;
		}
		return FALSE;

	default:
		return FALSE;
	}
}


HWND CreateSetTileset(HWND hwnd, LPMAPVIEWSETTINGS lpmvs) {
	DialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_DIALOGSETTILESET), hwnd, DlgProc, (LPARAM) lpmvs);
	return NULL;
}
