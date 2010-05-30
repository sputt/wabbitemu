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
#include "Layers.h"

#include "resource.h"

#define OBJECT_PROPERTIES_CLASS _T("ZMEObjectProperties")

extern HINSTANCE g_hInstance;
extern MAPSETSETTINGS g_MapSet;
extern LAYER g_Layer;

#define IDC_PROPERTY_BASE	(IDC_EDITX)
#define CX_MARGIN	20


static void GetArgumentValue(LPOBJECT lpo, int i, TCHAR *szValue, int cbValue) {
	TCHAR *name = lpo->lpType->szArgs[i][0];
	if (_tcsicmp(name, _T("x")) == 0) {
		StringCbPrintf(szValue, cbValue, _T("%1.lf"), lpo->x);
	} else if (_tcsicmp(name, _T("y")) == 0) {
		StringCbPrintf(szValue, cbValue, _T("%1.lf"), lpo->y);
	} else if (_tcsicmp(name, _T("z")) == 0) {
		StringCbPrintf(szValue, cbValue, _T("%1.lf"), lpo->z);
	} else if (_tcsicmp(name, _T("w")) == 0) {
		StringCbPrintf(szValue, cbValue, _T("%1.lf"), lpo->w);
	} else if (_tcsicmp(name, _T("h")) == 0) {
		StringCbPrintf(szValue, cbValue, _T("%1.lf"), lpo->h);
	} else {
		StringCbPrintf(szValue, cbValue, _T("%s"), lpo->szArgs[i]);
	}
}

static void SetArgumentValue(LPOBJECT lpo, int i, const TCHAR *szValue) {
	TCHAR *name = lpo->lpType->szArgs[i][0];
	if (_tcsicmp(name, _T("x")) == 0) {
		lpo->x = (double) _ttoi(szValue);
	} else if (_tcsicmp(name, _T("y")) == 0) {
		lpo->y = (double) _ttoi(szValue);
	} else if (_tcsicmp(name, _T("z")) == 0) {
		lpo->z = (double) _ttoi(szValue);
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
			LPOBJECT lpo;
			LPOBJECTTYPE lpot;
			HDC hdc;
			lpo = (LPOBJECT) lParam;
			lpot = lpo->lpType;
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR) lpo);

			POINT pt = {(lpo->x + lpo->w/2 + lpo->lpImage->cxImage/2 + 4) * g_MapSet.Scale, 
				(lpo->y - lpo->z + lpo->h - lpo->lpImage->cyImage) * g_MapSet.Scale};
			MapWindowPoints(((LPMAPVIEWSETTINGS) lpo->lpmvs)->hwndMap, NULL, &pt, 1);

			pt.y -= GetSystemMetrics(SM_CYCAPTION);
			SetWindowPos(hwnd, NULL, pt.x, pt.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

			TCHAR szValue[256];
			StringCbPrintf(szValue, sizeof(szValue), _T("%s Properties"), lpot->szName);
			SetWindowText(hwnd, szValue);

			hdc = GetDC(hwnd);
			SelectObject(hdc, g_Fonts.hfontMessage);
			GetTextMetrics(hdc, &tm);
			ReleaseDC(hwnd, hdc);

			RECT rc, wr1, wr2, sr;
			GetWindowRect(GetDlgItem(hwnd, IDC_EDITX), &wr1);
			GetWindowRect(GetDlgItem(hwnd, IDC_EDITY), &wr2);
			GetWindowRect(GetDlgItem(hwnd, IDC_STATICXPOSITION), &sr);
			int cyOffsetLabel = sr.top - wr1.top;
			cyRow = wr2.bottom - wr1.bottom;
			MapWindowRect(NULL, hwnd, &wr1);
			GetClientRect(hwnd, &rc);
			cxMargin = rc.right - wr1.right;

			for (int i = 0; i < lpot->ArgCount; i++) {
				HWND hwndChild;
				TCHAR szValue[256];

				if (i >= 2) {
					StringCbPrintf(szValue, sizeof(szValue), _T("%c%s:"), 
						toupper(lpot->szArgs[i][1][0]), &lpot->szArgs[i][1][1]);
					hwndChild = CreateWindow(_T("static"), szValue, WS_CHILD | WS_VISIBLE,
						cxMargin, cxMargin + (cyRow * i) + cyOffsetLabel,
						tm.tmAveCharWidth * 22, wr1.bottom - wr1.top,
						hwnd, (HMENU) -1, g_hInstance,  NULL);

					SetWindowFont(hwndChild, g_Fonts.hfontMessage, TRUE);

					GetArgumentValue(lpo, i, szValue, sizeof(szValue));
					int cxEdit = max(tm.tmAveCharWidth * max(lstrlen(szValue), 5) * 4 / 3, (wr1.right - wr1.left));
					// set the values (but you'll have to look it up)
					if (lstrcmp(lpo->lpType->szArgs[i][0], _T("type")) == 0) {
						cxEdit = tm.tmAveCharWidth * 19;
						hwndChild = CreateWindowEx(0, _T("COMBOBOX"), _T(""), WS_CHILD | WS_VISIBLE | CBS_SIMPLE | CBS_DROPDOWN |  WS_VSCROLL,
							rc.right - cxMargin - cxEdit, cxMargin + (cyRow * i),
							cxEdit, wr1.bottom - wr1.top,
							hwnd, (HMENU) (IDC_PROPERTY_BASE + i), g_hInstance, NULL);
						SendMessage(hwndChild, CB_SETDROPPEDWIDTH, 28 * tm.tmAveCharWidth, NULL);
						PopulateComboBoxWithObjectIDs(hwndChild);
						ComboBox_SelectByItemData(hwndChild, (LONG_PTR) szValue);
					} else if (lstrcmp(lpo->lpType->szArgs[i][0], _T("ap")) == 0) {
						cxEdit = tm.tmAveCharWidth * 19;
						hwndChild = CreateWindowEx(0, _T("COMBOBOX"), _T(""), WS_CHILD | WS_VISIBLE | CBS_SIMPLE | CBS_DROPDOWN |  WS_VSCROLL,
							rc.right - cxMargin - cxEdit, cxMargin + (cyRow * i),
							cxEdit, wr1.bottom - wr1.top,
							hwnd, (HMENU) (IDC_PROPERTY_BASE + i), g_hInstance, NULL);
						SendMessage(hwndChild, CB_SETDROPPEDWIDTH, 28 * tm.tmAveCharWidth, NULL);
						PopulateComboBoxWithGraphics(hwndChild);
						ComboBox_SelectString(hwndChild, -1, szValue);
					} else if (lstrcmp(lpo->lpType->szArgs[i][0], _T("g")) == 0) {
						cxEdit = tm.tmAveCharWidth * 12;
						hwndChild = CreateWindowEx(0, _T("COMBOBOX"), _T(""), WS_CHILD | WS_VISIBLE | CBS_SIMPLE | CBS_DROPDOWN |  WS_VSCROLL,
							rc.right - cxMargin - cxEdit, cxMargin + (cyRow * i),
							cxEdit, wr1.bottom - wr1.top,
							hwnd, (HMENU) (IDC_PROPERTY_BASE + i), g_hInstance, NULL);
						ComboBox_AddString(hwndChild, _T("d_down"));
						ComboBox_AddString(hwndChild, _T("d_left"));
						ComboBox_AddString(hwndChild, _T("d_right"));
						ComboBox_AddString(hwndChild, _T("d_up"));
						ComboBox_AddString(hwndChild, _T("d_none"));

						ComboBox_SelectString(hwndChild, -1, szValue);
					} else {
						hwndChild = CreateWindowEx(WS_EX_CLIENTEDGE, _T("edit"), szValue, WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | (isalpha(szValue[0]) ? ES_LEFT : ES_RIGHT),
							rc.right - cxMargin - cxEdit, cxMargin + (cyRow * i),
							cxEdit, wr1.bottom - wr1.top,
							hwnd, (HMENU) (IDC_PROPERTY_BASE + i), g_hInstance, NULL);
					}
					SendMessage(hwndChild, WM_SETFONT, (WPARAM) g_Fonts.hfontMessage, TRUE);
				} else {
					GetArgumentValue(lpo, i, szValue, sizeof(szValue));
					if (i == 0) {
						hwndChild = GetDlgItem(hwnd, IDC_EDITX);
					} else if (i == 1) {
						hwndChild = GetDlgItem(hwnd, IDC_EDITY);
					}
					Edit_SetText(hwndChild, szValue);
				}
			}

			SendMessage(hwnd, WM_SIZE, 0, 0);
			ShowWindow(hwnd, SW_SHOW);
			return TRUE;
		}

	case WM_SIZE:
		{
			LPOBJECT lpo = (LPOBJECT) GetWindowLongPtr(hwnd, GWLP_USERDATA);
			RECT rc, wr;
			GetWindowRect(hwnd, &wr);

			int cyCaption = GetSystemMetrics(SM_CYCAPTION);
			SetWindowPos(hwnd, NULL, 0, 0, wr.right - wr.left, 
				(cxMargin * 3) + (cyRow * (lpo->lpType->ArgCount + 1)) + cyCaption, 
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
					LPOBJECT lpo = (LPOBJECT) GetWindowLongPtr(hwnd, GWLP_USERDATA);

					SaveUndoStep(lpo->lpmvs->hwndMap);
					for (int i = 0; i < lpo->lpType->ArgCount; i++) {
						TCHAR szValue[256];
						TCHAR szWindowClass[256];
						GetClassName(GetDlgItem(hwnd, IDC_PROPERTY_BASE+i), szWindowClass, sizeof(szWindowClass) / sizeof(szWindowClass));
						if (_tcsicmp(szWindowClass, _T("combobox")) == 0) {
							TCHAR *psz = (TCHAR *) ComboBox_GetSelectionItemData(GetDlgItem(hwnd, IDC_PROPERTY_BASE+i));
							SetArgumentValue(lpo, i, psz);
						} else {
							GetWindowText(GetDlgItem(hwnd, IDC_PROPERTY_BASE+i), szValue, sizeof(szValue) / sizeof(TCHAR));
							SetArgumentValue(lpo, i, szValue);
						}
					}

				}
				// Fall through
			case IDCANCEL:
				EndDialog(hwnd, 0);
				return TRUE;
			}
			break;
		}
		return FALSE;

	case WM_CLOSE:
		EndDialog(hwnd, 0);
		return TRUE;

	default:
		return FALSE;
	}
}




HWND CreateObjectProperties(HWND hwndParent, LPOBJECT lpo) {
	DialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_DIALOGOBJECTPROPERTIES), hwndParent, DlgProc, (LPARAM) lpo);
	char szOutput[256];
	CreateObjectOutput(lpo, szOutput, sizeof(szOutput), TRUE);

	ZENEMY zobj = {0};
	RunAssemblyWithArguments(szOutput, (LPBYTE) &zobj, sizeof(zobj));
	
	lpo->x = zobj.x;
	lpo->w = zobj.w;
	lpo->y = zobj.y;
	lpo->h = zobj.h;
	lpo->z = zobj.z;
	lpo->d = zobj.d;

	// Due to a terrible design choice in zelda, animations are located in different places for objects and enemies
	if (g_Layer == ENEMY_LAYER) {
		lpo->lpImage = GetObjectImage(zobj.anim_ptr);
	} else if (g_Layer == OBJECT_LAYER) {
		lpo->lpImage = GetObjectImage(((ZOBJECT *) &zobj)->anim_ptr);
	}
	
	return NULL;
}