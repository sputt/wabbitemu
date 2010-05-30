#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include "MapHierarchy.h"
#include "MapView.h"

#include "resource.h"

extern MAPSETSETTINGS g_MapSet;
extern HINSTANCE g_hInstance;

static INT_PTR CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg)
	{
	case WM_INITDIALOG:
		{
			MAPVIEWSETTINGS mvs;
			GetMapViewSettings(g_MapSet.hwndSelected, &mvs);
			Edit_SetText(GetDlgItem(hwnd, IDC_EDITMAPNAME), mvs.szName);
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
					MAPVIEWSETTINGS mvs;
					GetMapViewSettings(g_MapSet.hwndSelected, &mvs);

					Edit_GetText(GetDlgItem(hwnd, IDC_EDITMAPNAME), mvs.szName, sizeof(mvs.szName));
					SetMapViewSettings(g_MapSet.hwndSelected, &mvs);
				}
				// fall through
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

HWND CreateMapProperties(HWND hwndParent) {
	return (HWND) DialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_DIALOGMAPPROPERTIES), hwndParent, DlgProc, NULL);
}