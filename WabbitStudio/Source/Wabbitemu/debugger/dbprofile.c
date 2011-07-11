#include "stdafx.h"
#include "dbprofile.h"
#include "calc.h"
#include "resource.h"
#include "dbcommon.h"

LRESULT CALLBACK ProfileDialogProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	switch(Message) {
		case WM_INITDIALOG:
			return TRUE;
		case WM_COMMAND: {
			switch(LOWORD(wParam)) {
				case IDC_PROFILE_OK: {
					char string[9];
					int output;
					HWND hWndEdit;
					hWndEdit = GetDlgItem(hwnd, IDC_LOW_EDT);
					SendMessage(hWndEdit, WM_GETTEXT, 8, (LPARAM)&string);
					xtoi((const TCHAR *)&string, &output);
					lpDebuggerCalc->profiler.lowAddress = output;
					hWndEdit = GetDlgItem(hwnd, IDC_HIGH_EDT);
					SendMessage(hWndEdit, WM_GETTEXT, 8, (LPARAM)&string);
					xtoi((const TCHAR *)&string, &output);
					lpDebuggerCalc->profiler.highAddress = output;
					hWndEdit = GetDlgItem(hwnd, IDC_BLOCK_EDT);
					SendMessage(hWndEdit, WM_GETTEXT, 8, (LPARAM)&string);
					output = _tstoi((const TCHAR *) &string);
					lpDebuggerCalc->profiler.blockSize = output;
					EndDialog(hwnd, IDOK);
					break;
				}
				case IDC_PROFILE_CANCEL:
					EndDialog(hwnd, IDCANCEL);
					break;
			}
			break;
		}
		case WM_CLOSE:
			EndDialog(hwnd, IDCANCEL);
			return FALSE;
	}
	return FALSE;//DefWindowProc(hwnd, Message, wParam, lParam);
}