#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <tchar.h>
#include <strsafe.h>

#include "resource.h"
#include "MapView.h"
#include "ObjectSelection.h"
#include "Misc.h"
#include "UndoRedo.h"

extern HINSTANCE g_hInstance;
extern MISCTYPE MiscTypes[256];
extern DWORD MiscTypeCount;

static LPMAPVIEWSETTINGS lpmvs;
static LPVOID ObjectArray[MAX_OBJECT];
static DWORD ObjectCount;

static int nTabSel = 1;

static void PopulateObjectArray(LPMAPVIEWSETTINGS lpmvs, int iTab)
{
	switch (iTab)
	{
	case 0:
		ObjectCount = lpmvs->AnimateCount;
		break;
	case 1:
		ObjectCount = lpmvs->ObjectCount;
		break;
	case 2:
		ObjectCount = lpmvs->EnemyCount;
		break;
	case 3:
		ObjectCount = lpmvs->MiscCount;
		break;
	}

	for (int i = 0; i < ObjectCount; i++) {
		
		switch (iTab)
		{
		case 0:
			ObjectArray[i] = &lpmvs->AnimateArray[i];
			break;
		case 1:
			ObjectArray[i] = &lpmvs->ObjectArray[i];
			break;
		case 2:
			ObjectArray[i] = &lpmvs->EnemyArray[i];
			break;
		case 3:
			ObjectArray[i] = &lpmvs->MiscArray[i];
			break;
		}
	}
}

static void PopulateListBox(HWND hwndList, int iTab)
{
	ListBox_ResetContent(hwndList);
	for (int i = 0; i < ObjectCount; i++) {
		TCHAR szObjectName[256];
		StringCbPrintf(szObjectName, sizeof(szObjectName), _T("%02d - "), i);

		switch (iTab)
		{
		case 0:
			StringCbCat(szObjectName, sizeof(szObjectName), ((LPANIMATE) ObjectArray[i])->lpType->szName);
			break;
		case 1:
			StringCbCat(szObjectName, sizeof(szObjectName), ((LPOBJECT) ObjectArray[i])->lpType->szName);
			break;
		case 2:
			StringCbCat(szObjectName, sizeof(szObjectName), ((LPOBJECT) ObjectArray[i])->lpType->szName);
			break;
		case 3:
			StringCbCat(szObjectName, sizeof(szObjectName), MiscTypes[((LPMISC) ObjectArray[i])->MiscID].szName);
			break;
		}

		ListBox_AddString(hwndList, szObjectName);
	}
}

static INT_PTR CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg)
	{
	case WM_INITDIALOG:
		{
			HWND hwndTab = GetDlgItem(hwnd, IDC_TAB);

			SetWindowLongPtr(hwnd, GWLP_USERDATA, lParam);
			HWND hwndList = GetDlgItem(hwnd, IDC_LIST);

			TCITEM tci = {0};
			tci.mask = TCIF_TEXT;

			tci.pszText = _T("Animate");
			TabCtrl_InsertItem(hwndTab, 0, &tci);

			tci.pszText = _T("Objects");
			TabCtrl_InsertItem(hwndTab, 1, &tci);

			tci.pszText = _T("Enemies");
			TabCtrl_InsertItem(hwndTab, 2, &tci);

			tci.pszText = _T("Misc");
			TabCtrl_InsertItem(hwndTab, 3, &tci);

			TabCtrl_SetCurSel(hwndTab, nTabSel);
			PopulateObjectArray((LPMAPVIEWSETTINGS) lParam, nTabSel);
			PopulateListBox(hwndList, nTabSel);
			return TRUE;
		}

	case WM_COMMAND:
		switch (HIWORD(wParam))
		{
		case BN_CLICKED:
			switch (LOWORD(wParam))
			{
			case IDC_BUTTONUP:
				{
					HWND hwndList = GetDlgItem(hwnd, IDC_LIST);
					int iSel = ListBox_GetCurSel(hwndList);
					if (iSel == 0) {
						return FALSE;
					}

					LPOBJECT lpoBackup = (LPOBJECT) ObjectArray[iSel];
					ObjectArray[iSel] = ObjectArray[iSel - 1];
					ObjectArray[iSel - 1] = lpoBackup;

					PopulateListBox(hwndList, TabCtrl_GetCurSel(GetDlgItem(hwnd, IDC_TAB)));
					ListBox_SetCurSel(hwndList, iSel - 1);
					return TRUE;
				}

			case IDC_BUTTONDOWN:
				{
					HWND hwndList = GetDlgItem(hwnd, IDC_LIST);
					int iSel = ListBox_GetCurSel(hwndList);
					if (iSel == ListBox_GetCount(hwndList) - 1) {
						return FALSE;
					}

					LPOBJECT lpoBackup = (LPOBJECT) ObjectArray[iSel];
					ObjectArray[iSel] = ObjectArray[iSel + 1];
					ObjectArray[iSel + 1] = lpoBackup;

					PopulateListBox(hwndList, TabCtrl_GetCurSel(GetDlgItem(hwnd, IDC_TAB)));
					ListBox_SetCurSel(hwndList, iSel + 1);
					return TRUE;
				}

			case IDCANCEL:
				EndDialog(hwnd, IDCANCEL);
				return TRUE;

			case IDOK:
				SaveUndoStep(lpmvs->hwndMap);

				for (int i = 0; i < ObjectCount; i++) {
					switch (TabCtrl_GetCurSel(GetDlgItem(hwnd, IDC_TAB)))
					{
					case 0:
						lpmvs->AnimateArray[i] = *(LPANIMATE) ObjectArray[i];
						break;
					case 1:
						lpmvs->ObjectArray[i] = *(LPOBJECT) ObjectArray[i];
						break;
					case 2:
						lpmvs->EnemyArray[i] = *(LPOBJECT) ObjectArray[i];
						break;
					case 3:
						lpmvs->MiscArray[i] = *(LPMISC) ObjectArray[i];
						break;
					}
				}
				nTabSel = TabCtrl_GetCurSel(GetDlgItem(hwnd, IDC_TAB));
				EndDialog(hwnd, IDOK);
				return TRUE;
			}
			break;
		case LBN_SELCHANGE:
			{
				LPMAPVIEWSETTINGS lpmvs = (LPMAPVIEWSETTINGS) GetWindowLongPtr(hwnd, GWLP_USERDATA);
				ClearObjectSelection();
				AddObjectToSelection(ObjectArray[ListBox_GetCurSel(GetDlgItem(hwnd, IDC_LIST))]);
				InvalidateRect(lpmvs->hwndMap, NULL, FALSE);
				UpdateWindow(lpmvs->hwndMap);
				return TRUE;
			}
		}
		return FALSE;

	case WM_NOTIFY:
		switch (((LPNMHDR) lParam)->code)
		{
		case TCN_SELCHANGE:
			{
				PopulateObjectArray((LPMAPVIEWSETTINGS) GetWindowLongPtr(hwnd, GWLP_USERDATA), TabCtrl_GetCurSel(GetDlgItem(hwnd, IDC_TAB)));
				PopulateListBox(GetDlgItem(hwnd, IDC_LIST), TabCtrl_GetCurSel(GetDlgItem(hwnd, IDC_TAB)));
				return TRUE;
			}
		}
		return FALSE;

	default:
		return FALSE;
	}
}

int ManageObjectOrder(HWND hwndParent, const LPMAPVIEWSETTINGS lpmvs_ref, LPMAPVIEWSETTINGS lpmvs_arg) {
	lpmvs = lpmvs_arg;
	return DialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_DIALOGMANAGEOBJECTORDER), hwndParent, DlgProc, (LPARAM) lpmvs_ref);
}
