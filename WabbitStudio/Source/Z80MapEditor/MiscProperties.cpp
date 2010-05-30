#include <windows.h>
#include <windowsx.h>
#include <commdlg.h>
#include <string.h>
#include <tchar.h>
#include <strsafe.h>
#include "MapObjects.h"
#include "Fonts.h"
#include "MapHierarchy.h"
#include "spasm.h"
#include "ObjectIDs.h"
#include "ObjectGraphics.h"
#include "utils.h"
#include "UndoRedo.h"

#include "resource.h"

#define IDC_PROPERTY_BASE	(IDC_EDITX)

extern HINSTANCE g_hInstance;
extern MAPSETSETTINGS g_MapSet;

extern MISCTYPE MiscTypes[256];
extern DWORD MiscTypeCount;

static BOOL fFirstOpen = FALSE;

static void GetArgumentValue(LPMISC lpo, int i, TCHAR *szValue, int cbValue) {
	TCHAR *name = MiscTypes[lpo->MiscID].szArgs[i][0];
	if (_tcsicmp(name, _T("x")) == 0) {
		StringCbPrintf(szValue, cbValue, _T("%1.lf"), lpo->x);
	} else if (_tcsicmp(name, _T("y")) == 0) {
		StringCbPrintf(szValue, cbValue, _T("%1.lf"), lpo->y);
	} else if (_tcsicmp(name, _T("w")) == 0) {
		StringCbPrintf(szValue, cbValue, _T("%1.lf"), lpo->w);
	} else if (_tcsicmp(name, _T("h")) == 0) {
		StringCbPrintf(szValue, cbValue, _T("%1.lf"), lpo->h);
	} else {
		StringCbPrintf(szValue, cbValue, _T("%s"), lpo->szArgs[i]);
	}
}

static void SetArgumentValue(LPMISC lpo, int i, const TCHAR *szValue) {
	TCHAR *name = MiscTypes[lpo->MiscID].szArgs[i][0];
	if (_tcsicmp(name, _T("x")) == 0) {
		lpo->x = (double) _ttoi(szValue);
	} else if (_tcsicmp(name, _T("y")) == 0) {
		lpo->y = (double) _ttoi(szValue);
	} else if (_tcsicmp(name, _T("w")) == 0) {
		lpo->w = (double) _ttoi(szValue);
	} else if (_tcsicmp(name, _T("h")) == 0) {
		lpo->h = (double) _ttoi(szValue);
	} else {
		StringCbCopy(lpo->szArgs[i], sizeof(lpo->szArgs[i]), szValue);
	}
}

static INT_PTR CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	static TEXTMETRIC tm = {0};
	static DWORD cxMargin, cyRow;

	switch (uMsg)
	{
	case WM_INITDIALOG:
		{
			LPMISC lpm = (LPMISC) lParam;
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR) lpm);

			POINT pt = {(lpm->x + lpm->w + 4) * g_MapSet.Scale, 
			lpm->y * g_MapSet.Scale};
			MapWindowPoints(((LPMAPVIEWSETTINGS) lpm->lpmvs)->hwndMap, NULL, &pt, 1);

			if (fFirstOpen == TRUE) {
				pt.y -= GetSystemMetrics(SM_CYCAPTION);
				SetWindowPos(hwnd, NULL, pt.x, pt.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
			}

			HDC hdc = GetDC(hwnd);
			SelectObject(hdc, g_Fonts.hfontMessage);
			GetTextMetrics(hdc, &tm);

			RECT rc, wr1, wr2, sr;
			GetWindowRect(GetDlgItem(hwnd, IDC_EDITX), &wr1);
			GetWindowRect(GetDlgItem(hwnd, IDC_EDITY), &wr2);
			GetWindowRect(GetDlgItem(hwnd, IDC_STATICXPOSITION), &sr);
			int cyOffsetLabel = sr.top - wr1.top;
			cyRow = wr2.bottom - wr1.bottom;
			MapWindowRect(NULL, hwnd, &wr1);
			GetClientRect(hwnd, &rc);
			cxMargin = rc.right - wr1.right;

			ComboBox_ResetContent(GetDlgItem(hwnd, IDC_COMBOTYPE));
			for (DWORD i = 0; i < MiscTypeCount; i++) {
				ComboBox_AddString(GetDlgItem(hwnd, IDC_COMBOTYPE), MiscTypes[i].szName);
			}
			ComboBox_SetCurSel(GetDlgItem(hwnd, IDC_COMBOTYPE), lpm->MiscID);

			SetDlgItemInt(hwnd, IDC_EDITX, lpm->x, FALSE);
			SetDlgItemInt(hwnd, IDC_EDITY, lpm->y, FALSE);
			SetDlgItemInt(hwnd, IDC_EDITW, lpm->w, FALSE);
			SetDlgItemInt(hwnd, IDC_EDITH, lpm->h, FALSE);

			int cxStatic = -1;
			LPMISCTYPE lpmt = &MiscTypes[lpm->MiscID];
			BOOL fLeftBuffer[64];
			for (int i = 0; i < ARRAYSIZE(fLeftBuffer); i++) {
				fLeftBuffer[i] = FALSE;
			}
			for (int i = 4; i < lpmt->ArgCount; i++) {
				HWND hwndChild;
				TCHAR szValue[256];
				StringCbPrintf(szValue, sizeof(szValue), _T("%c%s:"), 
					toupper(lpmt->szArgs[i][1][0]), &lpmt->szArgs[i][1][1]);

				SIZE Size;
				GetTextExtentExPoint(hdc, szValue, lstrlen(szValue), rc.right, NULL, NULL, &Size);
				
				RECT rLabel = {0, 0, Size.cx, Size.cy};
				MapDialogRect(hwnd, &rLabel);
				cxStatic = cxMargin + max((rLabel.right - rLabel.left), cxStatic);

				hwndChild = CreateWindow(_T("static"), szValue, WS_CHILD | WS_VISIBLE,
					cxMargin, cxMargin + (cyRow * (i + 1)) + cyOffsetLabel,
					rLabel.right - rLabel.left, wr1.bottom - wr1.top,
					hwnd, (HMENU) -1, g_hInstance,  NULL);

				SetWindowFont(hwndChild, g_Fonts.hfontMessage, TRUE);
				SetWindowPos(hwndChild, GetPrevSibling(GetDlgItem(hwnd, IDOK)), 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

				fLeftBuffer[i] = (isdigit(szValue[0]) ? FALSE : TRUE);
				if (szValue[0] == 'X' || szValue[0] == 'Y') {
					fLeftBuffer[i] = FALSE;
				}

				GetArgumentValue(lpm, i, szValue, sizeof(szValue));
				int cxEdit = max(tm.tmAveCharWidth * max(lstrlen(szValue), 5) * 4 / 3, (wr1.right - wr1.left));
				// set the values (but you'll have to look it up)

				hwndChild = CreateWindowEx(WS_EX_CLIENTEDGE, _T("edit"), szValue, WS_TABSTOP | WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | (fLeftBuffer[i] ? ES_LEFT : ES_RIGHT),
					rc.right - cxMargin - cxEdit, cxMargin + (cyRow * (i + 1)),
					cxEdit, wr1.bottom - wr1.top,
					hwnd, (HMENU) (IDC_PROPERTY_BASE + i), g_hInstance, NULL);
				SetWindowPos(hwndChild, GetPrevSibling(GetDlgItem(hwnd, IDOK)), 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

				SendMessage(hwndChild, WM_SETFONT, (WPARAM) g_Fonts.hfontMessage, TRUE);
			}
			ReleaseDC(hwnd, hdc);

			for (int i = 4; i < lpmt->ArgCount; i++) {
				if (fLeftBuffer[i] == TRUE) {
					SetWindowPos(GetDlgItem(hwnd, IDC_PROPERTY_BASE + i), NULL, rc.right - cxMargin - (rc.right - cxStatic), cxMargin + (cyRow * (i + 1)), 
						rc.right - cxStatic, wr1.bottom - wr1.top, SWP_NOZORDER);
				}
			}

			SendMessage(hwnd, WM_SIZE, 0, 0);
			ShowWindow(hwnd, SW_SHOW);
			return TRUE;
		}

	case WM_SIZE:
		{
			LPMISC lpm = (LPMISC) GetWindowLongPtr(hwnd, GWLP_USERDATA);
			
			RECT rc, wr;
			GetWindowRect(hwnd, &wr);

			int cyCaption = GetSystemMetrics(SM_CYCAPTION);
			SetWindowPos(hwnd, NULL, 0, 0, wr.right - wr.left, 
				(cxMargin * 3) + (cyRow * (MiscTypes[lpm->MiscID].ArgCount + 2)) + cyCaption, 
				SWP_NOMOVE | SWP_NOZORDER);

			RECT br;
			GetWindowRect(GetDlgItem(hwnd, IDCANCEL), &br);

			GetClientRect(hwnd, &rc);
			SetWindowPos(GetDlgItem(hwnd, IDOK), NULL, 
				rc.right - (br.right - br.left)*2 - cxMargin * 5 / 3,
				rc.bottom - cxMargin - cyRow,
				0, 0, SWP_NOSIZE | SWP_NOZORDER);
			SetWindowPos(GetDlgItem(hwnd, IDCANCEL), NULL,
				rc.right - cxMargin - (br.right - br.left),
				rc.bottom - cxMargin - cyRow,
				0, 0, SWP_NOSIZE | SWP_NOZORDER);
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
					LPMISC lpm = (LPMISC) GetWindowLongPtr(hwnd, GWLP_USERDATA);

					SaveUndoStep(lpm->lpmvs->hwndMap);

					for (int i = 0; i < MiscTypes[lpm->MiscID].ArgCount; i++) {
						TCHAR szValue[256];
						TCHAR szWindowClass[256];
						HWND hwndChild = GetDlgItem(hwnd, IDC_PROPERTY_BASE + i);
						GetClassName(hwndChild, szWindowClass, ARRAYSIZE(szWindowClass));
						if (_tcsicmp(szWindowClass, _T("combobox")) == 0) {
							TCHAR *psz = (TCHAR *) ComboBox_GetSelectionItemData(GetDlgItem(hwnd, IDC_PROPERTY_BASE+i));
							if (psz == NULL) {
								psz = _T("");
							}
							SetArgumentValue(lpm, i, psz);
						} else {
							GetWindowText(GetDlgItem(hwnd, IDC_PROPERTY_BASE+i), szValue, ARRAYSIZE(szValue));
							SetArgumentValue(lpm, i, szValue);
						}
					}

				}
				// Fall through
			case IDCANCEL:
				EndDialog(hwnd, 0);
				return TRUE;
			}
			break;

		case CBN_SELCHANGE:
			{
				switch (GetDlgCtrlID((HWND) lParam))
				{
				case IDC_COMBOTYPE:
					{
						LPMISC lpm = (LPMISC) GetWindowLongPtr(hwnd, GWLP_USERDATA);

						for (int i = 4; i < MiscTypes[lpm->MiscID].ArgCount; i++) {
							DestroyWindow(GetPrevSibling(GetDlgItem(hwnd, IDC_PROPERTY_BASE+i)));
							DestroyWindow(GetDlgItem(hwnd, IDC_PROPERTY_BASE+i));
						}
						lpm->MiscID = ComboBox_GetCurSel((HWND) lParam);
						fFirstOpen = FALSE;
						SendMessage(hwnd, WM_INITDIALOG, 0, (LPARAM) lpm);
						return TRUE;
					}
				}
				break;
			}
		}
		return FALSE;

	case WM_CLOSE:
		EndDialog(hwnd, 0);
		return TRUE;

	default:
		return FALSE;
	}
}

HWND CreateMiscProperties(HWND hwndParent, LPMISC lpm) {
	fFirstOpen = TRUE;
	DialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_DIALOGMISCPROPERTIES), hwndParent, DlgProc, (LPARAM) lpm);	
	return NULL;
}