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
					TCHAR string[9];
					int output;
					HWND hWndEdit;
					GetDlgItemText(hwnd, IDC_BLOCK_EDT, string, 8);
					output = _tstoi(string);
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