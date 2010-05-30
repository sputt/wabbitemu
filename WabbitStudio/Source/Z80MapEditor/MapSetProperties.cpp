#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include "MapHierarchy.h"

#include "resource.h"

extern MAPSETSETTINGS g_MapSet;
extern HINSTANCE g_hInstance;

static INT_PTR CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg)
	{
	case WM_INITDIALOG:
		Edit_SetText(GetDlgItem(hwnd, IDC_EDITMAPSETNAME), g_MapSet.szName);
		return TRUE;

	case WM_COMMAND:
		switch (HIWORD(wParam))
		{
		case BN_CLICKED:
			switch (LOWORD(wParam))
			{
			case IDOK:
				Edit_GetText(GetDlgItem(hwnd, IDC_EDITMAPSETNAME), g_MapSet.szName, sizeof(g_MapSet.szName));
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

HWND CreateMapSetProperties(HWND hwndParent) {
	DialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_DIALOGMAPSETPROPERTIES), hwndParent, DlgProc, NULL);
	return NULL;
}